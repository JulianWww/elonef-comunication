#pragma once

#include "return_handler.hpp"
#include <functional>

namespace Elonef {
    template<typename T, typename WS>
    class CallbackReturnHandler : public ReturnHandler {
        private: std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback;
        private: std::function<void(T*, WS*)> deleter;
        private: T* _handler;
        private: WS* connection;

        public: CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn);
        public: CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn, std::function<void(T* handler, WS* connection)> deleter);
        public: ~CallbackReturnHandler();

        public: virtual void handle(CryptoPP::ByteQueue& content);

        private: static void defaultDeleter(T* handler, WS* connection);
    };
}

template<typename T, typename WS>
inline Elonef::CallbackReturnHandler<T, WS>::CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn) : CallbackReturnHandler(callback, handler, conn, defaultDeleter) {
}

template<typename T, typename WS>
inline Elonef::CallbackReturnHandler<T, WS>::CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn, std::function<void(T* handler, WS* connection)> deleter) {
    this->callback = callback;
    this->deleter = deleter;
    this->_handler = handler;
    this->connection = conn;
}

template<typename T, typename WS>
Elonef::CallbackReturnHandler<T, WS>::~CallbackReturnHandler() {
    this->deleter(this->_handler, this->connection);
}

template<typename T, typename WS>
inline void Elonef::CallbackReturnHandler<T, WS>::handle(CryptoPP::ByteQueue& content) {
    callback(this->_handler, this->connection, content);
}

template<typename T, typename WS>
inline void Elonef::CallbackReturnHandler<T, WS>::defaultDeleter(T* handler, WS* connection) {
}