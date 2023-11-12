#pragma once

#include "return_handler.hpp"
#include <functional>

namespace Elonef {
    template<typename T, typename WS>
    class CallbackReturnHandler : public ReturnHandler {
        private: std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback;
        private: T* _handler;
        private: WS* connection;

        public: CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn);

        public: virtual void handle(CryptoPP::ByteQueue& content);
    };
}

template<typename T, typename WS>
inline Elonef::CallbackReturnHandler<T, WS>::CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn) {
    this->callback = callback;
    this->_handler = handler;
    this->connection = conn;
}

template<typename T, typename WS>
inline void Elonef::CallbackReturnHandler<T, WS>::handle(CryptoPP::ByteQueue& content) {
    callback(this->_handler, this->connection, content);
}