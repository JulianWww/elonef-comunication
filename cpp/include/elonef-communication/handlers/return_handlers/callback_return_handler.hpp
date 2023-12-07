#pragma once

#include "return_handler.hpp"
#include <functional>

namespace Elonef {
    template<typename T, typename WS>
    class CallbackReturnHandler : public ReturnHandler {
        private: std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback;
        private: T* _handler;
        private: WS* connection;
        private: bool delHandler;
        private: bool delconn;

        public: CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn);
        public: CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn, bool delHandler, bool delconn);
        public: virtual ~CallbackReturnHandler();

        public: virtual void handle(CryptoPP::ByteQueue& content);
        public: virtual void reject(std::__exception_ptr::exception_ptr error);
    };
}

template<typename T, typename WS>
inline Elonef::CallbackReturnHandler<T, WS>::CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn) : CallbackReturnHandler(callback, handler, conn, false, false) {}

template<typename T, typename WS>
inline Elonef::CallbackReturnHandler<T, WS>::CallbackReturnHandler(std::function<void(T*, WS*, CryptoPP::ByteQueue&)> callback, T* handler, WS* conn, bool delHandler, bool delconn) {
    this->callback = callback;
    this->_handler = handler;
    this->connection = conn;
    this->delHandler = delHandler;
    this->delconn = delconn;
}

template<typename T, typename WS>
Elonef::CallbackReturnHandler<T, WS>::~CallbackReturnHandler() {
    if (this->delHandler) {
        delete this->_handler;
    }
    if (this->delconn) {
        delete this->connection;
    }

}

template<typename T, typename WS>
inline void Elonef::CallbackReturnHandler<T, WS>::handle(CryptoPP::ByteQueue& content) {
    callback(this->_handler, this->connection, content);
}

template<typename T, typename WS>
inline void Elonef::CallbackReturnHandler<T, WS>:: reject(std::__exception_ptr::exception_ptr error) {
    // TODO
}