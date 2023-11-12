#pragma once

#include <bits/stdint-uintn.h>
#include <iostream>
#include <thread>
#include <cryptopp/secblock.h>
#include "../utils.hpp"
#include <websocket.h>
#include <future>
#include "../encoding/decoding.hpp"
#include "../encoding/uuid.hpp"
#include <bst/linked_tree.hpp>
#include "return_handlers/return_handler.hpp"
#include <mutex>
#include <chrono>

namespace Elonef
{
    template<typename WS, typename T, typename ConnData>
    class MessageHandler {
        public: typedef ReturnHandler HandlerFunc;

        private: T* _this;
        private: std::mutex uuid_map_mu;
        private: std::list<std::future<void>> running_handlers;
        private: BST::LinkedTree<HandlerFunc> return_uid_map;

        public: MessageHandler(T* _this);
        public: ~MessageHandler();

        protected: void handle_message(WS& ws, const uint8_t* data, const uint32_t size, ConnData& connData);
        private: static void _handle_message_s(MessageHandler* _this, WS* ws, const uint8_t* data, const uint32_t size, ConnData* connData);
        private: void _handle_message_switch(WS& ws, const uint8_t* data, const uint32_t& size, ConnData& connData);

        private: void handle_return(WS& ws, CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ConnData& connData);
        private: void handle_auth_caller(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ConnData& connData);
        private: CryptoPP::ByteQueue custom_handler_caller(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ConnData& connData);

        public: void send(WS& conn, CryptoPP::ByteQueue& queue, CryptoPP::byte msg_type, HandlerFunc* handle_func);
        public: void send(WS& conn, CryptoPP::ByteQueue& queue, CryptoPP::byte msg_type);
        public: void send(WS& conn, CryptoPP::byte msg_type);
        public: CryptoPP::SecByteBlock send(WS& conn, CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& data, CryptoPP::byte msg_type);
        public: void send(WS& conn, const CryptoPP::SecByteBlock& queue);

        protected: void clean_executors();
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

template<typename WS, typename T, typename ConnData>
inline Elonef::MessageHandler<WS, T, ConnData>::MessageHandler(T* _this_): _this(_this_) {}

template<typename WS, typename T, typename ConnData>
inline Elonef::MessageHandler<WS, T, ConnData>::~MessageHandler() {
    this->running_handlers.remove_if(
        &wair_for_future_is_ready
    );
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::handle_message(WS& ws, const uint8_t* data, uint32_t size, ConnData& connData) {
    running_handlers.push_back(std::async(_handle_message_s, this, &ws, data, size, &connData));
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::_handle_message_s(MessageHandler* _this, WS* ws, const uint8_t* data, const uint32_t size, ConnData* connData){
    _this->_handle_message_switch(*ws, data, size, *connData);
}


template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::_handle_message_switch(WS& ws, const uint8_t* _data, const uint32_t& size, ConnData& connData) {
    CryptoPP::ByteQueue content = Elonef::get_message(_data, size);

    CryptoPP::ByteQueue uuid = Elonef::extractConstantLengthQueue(content, ELONEF_UUID_SIZE);
    CryptoPP::byte type = Elonef::extreactByte(content);

    CryptoPP::ByteQueue return_data;

    switch (type) {
        case RETURN: {
            this->handle_return(ws, uuid, content, connData);
            return;
        };
        case API: {
            std::cout << "api" << std::endl;
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

    this->send(ws, uuid, return_data, 0);
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::handle_return(WS& ws, CryptoPP::ByteQueue& uuid_queue, CryptoPP::ByteQueue& content, ConnData& connData) {
    

    const CryptoPP::SecByteBlock uid = Elonef::toSecBlock(uuid_queue);

    this->uuid_map_mu.lock();
    HandlerFunc* func = this->return_uid_map.getAndRelease((char*) uid.data(), 128);
    this->uuid_map_mu.unlock();

    if (func == nullptr) {
        return;
    }

    this->uuid_map_mu.lock();
    this->return_uid_map.remove((char*) uid.data(), 128);
    this->uuid_map_mu.unlock();

    func->handle(content);
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::handle_auth_caller(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ConnData& connData) {
    _this->handle_auth(uuid, content, connData);
}

template<typename WS, typename T, typename ConnData>
inline CryptoPP::ByteQueue Elonef::MessageHandler<WS, T, ConnData>::custom_handler_caller(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ConnData& connData) {
    return _this->custom_handler(type, content, connData);
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::send(WS& conn, CryptoPP::ByteQueue& queue, CryptoPP::byte msg_type, HandlerFunc* handler) {
    CryptoPP::ByteQueue uuid = Elonef::uuid();
    const CryptoPP::SecByteBlock data = this->send(conn, uuid, queue, msg_type);

    this->uuid_map_mu.lock();
    this->return_uid_map.insert((char*)data.data(), 128, handler);
    this->uuid_map_mu.unlock();
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::send(WS& conn, CryptoPP::ByteQueue& queue, CryptoPP::byte msg_type) {
    CryptoPP::ByteQueue uuid = Elonef::uuid();
    this->send(conn, uuid, queue, msg_type);
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::send(WS& conn, CryptoPP::byte msg_type) {
    CryptoPP::ByteQueue data;
    this->send(conn, data, msg_type);
}

template<typename WS, typename T, typename ConnData>
inline CryptoPP::SecByteBlock Elonef::MessageHandler<WS, T, ConnData>::send(WS& conn, CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& data, CryptoPP::byte msg_type) {
    uuid.Put(msg_type);
    data.TransferAllTo(uuid);
    const CryptoPP::SecByteBlock to_send = Elonef::toSecBlock(uuid);
    this->send(conn, to_send);
    return to_send;
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::send(WS& conn, const CryptoPP::SecByteBlock& data) {
    conn.send(websocket::OPCODE_BINARY, data.data(), data.SizeInBytes());
}

template<typename WS, typename T, typename ConnData>
inline void Elonef::MessageHandler<WS, T, ConnData>::clean_executors() {
    this->running_handlers.remove_if(
        &check_if_future_is_ready<void>
    );
}