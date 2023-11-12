#pragma once

#include "return_handler.hpp"
#include "../../data_waiter.hpp"

namespace Elonef {
    class PromiseReturnHandler : public ReturnHandler {
        private: Elonef::DataWaiter<CryptoPP::ByteQueue> data;

        public: virtual void handle(CryptoPP::ByteQueue& content);
        public: CryptoPP::ByteQueue get();
    };
}

inline void Elonef::PromiseReturnHandler::handle(CryptoPP::ByteQueue& content) {
    this->data.set_value(content);
}

inline CryptoPP::ByteQueue  Elonef::PromiseReturnHandler::get() {
    return this->data.get();
}