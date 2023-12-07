#pragma once

#include <thread>
#include <mutex>
#include <iostream>
#include <future>
#include <cryptopp/secblock.h>
#include <chrono>
#include <bst/linked_tree.hpp>
#include <bits/stdint-uintn.h>
#include "return_handlers/return_handler.hpp"
#include "../utils.hpp"
#include "../error.hpp"
#include "../encoding/uuid.hpp"
#include "../encoding/encoding.hpp"
#include "../encoding/decoding.hpp"
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXConnectionState.h>
#include "../data_waiter.hpp"
#include "data/connection_data.hpp"


namespace Elonef
{
    template<typename T, typename ConnData>
    class MessageHandler {
        private: typedef std::function<CryptoPP::ByteQueue(CryptoPP::ByteQueue& content, ConnData& connData)> ApiCallbackFunc;

        private: std::mutex cleaning_mu;
        private: bool send_ready = false;

        private: T* _this;
        private: std::mutex uuid_map_mu;
        private: std::list<std::future<void>> running_handlers;
        public: BST::LinkedTree<ReturnHandler> return_uid_map;
        public: std::unordered_map<std::string, std::pair<bool, ApiCallbackFunc>> handler_map;

        public: MessageHandler(T* _this);
        public: ~MessageHandler();

        protected: void handle_message(ix::WebSocket& webSocket, const ix::WebSocketMessagePtr & msg);

        protected: void handle_message(ix::WebSocket& ws, const std::string& data, ConnData* connData);
        private: static void _handle_message_s(MessageHandler* _this, ix::WebSocket* ws, const std::string& data, ConnData* connData);
        private: void _handle_message_switch(ix::WebSocket& ws, const std::string& data, ConnData& connData);

        private: void handle_return(ix::WebSocket& ws, CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ConnData& connData);
        private: void handle_auth_caller(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ConnData& connData);
        private: CryptoPP::ByteQueue custom_handler_caller(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ConnData& connData);

        protected: void send_api_request(ix::WebSocket& conn, const std::string& call_id, CryptoPP::ByteQueue& data, ReturnHandler* handler);

        public: void send(ix::WebSocket& conn, CryptoPP::ByteQueue& queue, CryptoPP::byte msg_type, ReturnHandler* handle_func);
        public: void send(ix::WebSocket& conn, CryptoPP::ByteQueue& queue, CryptoPP::byte msg_type);
        public: void send(ix::WebSocket& conn, CryptoPP::byte msg_type);
        public: void send(ix::WebSocket& conn, CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& data, CryptoPP::byte msg_type);
        private: void send(ix::WebSocket& conn, const std::string& queue);
        private: void waitForReady() const;

        protected: void clean_executors();

        // api

        public: void add_authenticated_api_callback(const std::string& name, ApiCallbackFunc func);
        public: void add_api_callback(const std::string& name, ApiCallbackFunc func, bool authenticated=false);

        private: CryptoPP::ByteQueue handle_api_request(CryptoPP::ByteQueue& content, ConnData& data);
    };

    const CryptoPP::byte RETURN = 0x00;
    const CryptoPP::byte API = 0x01;
    const CryptoPP::byte AUTH = 0x02; 
} // namespace Elonef

template<typename T>
inline bool check_if_future_is_ready(std::future<T>& f) {
    return f.wait_until(std::chrono::system_clock::now()) == std::future_status::ready;
}

inline bool wair_for_future_is_ready(std::future<void>& f) {
    f.wait();
    return true;
}

template<typename T, typename ConnData>
inline Elonef::MessageHandler<T, ConnData>::MessageHandler(T* _this_): _this(_this_) {}

template<typename T, typename ConnData>
inline Elonef::MessageHandler<T, ConnData>::~MessageHandler() {
    for (std::future<void>& handler : this->running_handlers) {
        handler.wait();
    }
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::handle_message(ix::WebSocket & webSocket, const ix::WebSocketMessagePtr & msg) {   
    ConnData* connData = webSocket.getCustomData<ConnData>();
    connData->active_processes++;
    switch (msg->type) {
        case (ix::WebSocketMessageType::Open) : {
            this->send_ready = true;
            _this->onOpen(webSocket);
            connData->active_processes--;
            return;
        }
        case (ix::WebSocketMessageType::Close) : {
            _this->onClose(webSocket);
            connData->active_processes--;
            while (connData->active_processes != 0) {
                std::this_thread::yield();
            }
            
            return;
        }
        case (ix::WebSocketMessageType::Message) : {
            this->handle_message(webSocket, msg->str, connData);
            return;
        }
    };
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::handle_message(ix::WebSocket& ws, const std::string& data, ConnData* connData) {
    this->cleaning_mu.lock();
    running_handlers.push_back(std::async(_handle_message_s, this, &ws, data, connData));
    this->cleaning_mu.unlock();
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::_handle_message_s(MessageHandler* _this, ix::WebSocket* ws, const std::string& data, ConnData* connData){
    _this->_handle_message_switch(*ws, data, *connData);
    _this->clean_executors();
    connData->active_processes--;
}


template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::_handle_message_switch(ix::WebSocket& ws, const std::string& _data, ConnData& connData) {
    CryptoPP::ByteQueue content = Elonef::get_message(_data);

    CryptoPP::ByteQueue uuid = Elonef::extractConstantLengthQueue(content, ELONEF_UUID_SIZE);
    CryptoPP::byte type = Elonef::extreactByte(content);    

    CryptoPP::ByteQueue return_data;
    try {
        switch (type) {
            case RETURN: {
                this->handle_return(ws, uuid, content, connData);
                return;
            };
            case API: {
                return_data = this->handle_api_request(content, connData);
                break;
            };
            case AUTH: {
                this->handle_auth_caller(uuid, content, connData);
                return;
            };
            default: {
                return_data = this->custom_handler_caller(type, content, connData);
                break;
            }
        };
        return_data.Unget(0x01);
    }
    catch (const Elonef::ForwardedError& err) {
        return_data.Clear();
        return_data.Put(0x00);
        return_data.Put((CryptoPP::byte*)err.what(), strlen(err.what())); // unsafe
    }
    catch (...) {
        return_data.Clear();
        return_data.Put(0x00);
    }
    this->send(ws, uuid, return_data, RETURN);
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::handle_return(ix::WebSocket& ws, CryptoPP::ByteQueue& uuid_queue, CryptoPP::ByteQueue& content, ConnData& connData) {
    const CryptoPP::SecByteBlock uid = Elonef::toSecBlock(uuid_queue);
    
    this->uuid_map_mu.lock();
    ReturnHandler* func = this->return_uid_map.getAndRelease((char*) uid.data(), 128);

    if (func == nullptr) {
        this->uuid_map_mu.unlock();
        return;
    }

    this->return_uid_map.remove((char*) uid.data(), 128);
    this->uuid_map_mu.unlock();
    CryptoPP::byte return_code = extreactByte(content);

    switch (return_code) {
        case 0x00 : {
            RemoteError error = RemoteError(Elonef::toString(content));
            func->reject(error);
            break;
        }
        case 0x01 : {
            func->handle(content);
            break;
        }
        default : {
            std::runtime_error error = std::runtime_error("unknown success type");
            func->reject(error);
            break;
        }
    }
    delete func;
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::handle_auth_caller(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ConnData& connData) {
    _this->handle_auth(uuid, content, connData);
}

template<typename T, typename ConnData>
inline CryptoPP::ByteQueue Elonef::MessageHandler<T, ConnData>::custom_handler_caller(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ConnData& connData) {
    return _this->custom_handler(type, content, connData);
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::send_api_request(ix::WebSocket& conn, const std::string& call_id, CryptoPP::ByteQueue& data, ReturnHandler* handler) {
    CryptoPP::ByteQueue toSend = Elonef::toBytes(call_id);
    data.TransferAllTo(toSend);
    this->send(conn, toSend, API, handler);
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::send(ix::WebSocket& conn, CryptoPP::ByteQueue& queue, CryptoPP::byte msg_type, ReturnHandler* handler) {
    CryptoPP::ByteQueue uuid = Elonef::uuid();
    CryptoPP::ByteQueue uuid_backup(uuid);
    CryptoPP::SecByteBlock data = Elonef::toSecBlock(uuid_backup);
    this->send(conn, uuid, queue, msg_type);

    this->uuid_map_mu.lock();
    this->return_uid_map.insert((char*)data.data(), 128, handler);
    this->uuid_map_mu.unlock();
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::send(ix::WebSocket& conn, CryptoPP::ByteQueue& queue, CryptoPP::byte msg_type) {
    CryptoPP::ByteQueue uuid = Elonef::uuid();
    this->send(conn, uuid, queue, msg_type);
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::send(ix::WebSocket& conn, CryptoPP::byte msg_type) {
    CryptoPP::ByteQueue data;
    this->send(conn, data, msg_type);
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::send(ix::WebSocket& conn, CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& data, CryptoPP::byte msg_type) {
    uuid.Put(msg_type);
    data.TransferAllTo(uuid);
    const std::string to_send = Elonef::toString(uuid);
    this->send(conn, to_send);
    //return to_send;
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::send(ix::WebSocket& conn, const std::string& data) {
    this->waitForReady();
    conn.sendBinary(data);
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::waitForReady() const {
    while (!this->send_ready) {
        std::this_thread::yield();
    }
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::clean_executors() {
    if (!this->cleaning_mu.try_lock()) {
        return;
    }

    this->running_handlers.remove_if(
        &check_if_future_is_ready<void>
    );
    this->cleaning_mu.unlock();
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::add_authenticated_api_callback(const std::string& name, ApiCallbackFunc func) {
    this->add_api_callback(name, func, true);
}

template<typename T, typename ConnData>
inline void Elonef::MessageHandler<T, ConnData>::add_api_callback(const std::string& name, ApiCallbackFunc func, bool authenticated) {
    std::pair<bool, ApiCallbackFunc> content = {authenticated, func};
    this->handler_map.insert_or_assign(
        name,
        content
    );
}

template<typename T, typename ConnData>
inline CryptoPP::ByteQueue Elonef::MessageHandler<T, ConnData>::handle_api_request(CryptoPP::ByteQueue& content, ConnData& data) {
    std::string api_name = Elonef::toDynamicSizeString(content);
    auto callback = this->handler_map.find(api_name);
    
    if (callback == this->handler_map.end()) {
        throw ForwardedError("the api call \"" + api_name + "\" does not exist");
        return CryptoPP::ByteQueue();
    }

    if (callback->second.first && !T::is_authenticated(data)) {
        throw ForwardedError("user must be authenticated to make the api call \"" + api_name + "\"");
    }

    return callback->second.second(content, data);
}