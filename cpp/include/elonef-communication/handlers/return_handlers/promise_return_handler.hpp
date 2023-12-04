#pragma once

#include "return_handler.hpp"
#include "../../data_waiter.hpp"

namespace Elonef {
    class PromiseReturnHandler : public ReturnHandler {
        private: Elonef::DataWaiter<CryptoPP::ByteQueue>* data;

        public: PromiseReturnHandler(Elonef::DataWaiter<CryptoPP::ByteQueue>* data_waiter);

        public: virtual void handle(CryptoPP::ByteQueue& content);
        public: virtual void reject(std::__exception_ptr::exception_ptr exception);
    };
}

inline Elonef::PromiseReturnHandler::PromiseReturnHandler(Elonef::DataWaiter<CryptoPP::ByteQueue>* data_waiter) {
    this->data = data_waiter;
}

inline void Elonef::PromiseReturnHandler::handle(CryptoPP::ByteQueue& content) {
    this->data->set_value(content);
}

inline void Elonef::PromiseReturnHandler::reject(std::__exception_ptr::exception_ptr err) {
    data->reject(err);
}