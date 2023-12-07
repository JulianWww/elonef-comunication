#pragma once

#include "return_handler.hpp"
#include "../../data_waiter.hpp"
#include <memory>
#include <iostream>

namespace Elonef {
    class PromiseReturnHandler : public ReturnHandler {
        private: std::shared_ptr<Elonef::DataWaiter<CryptoPP::ByteQueue>> data;

        public: PromiseReturnHandler(std::shared_ptr<Elonef::DataWaiter<CryptoPP::ByteQueue>> data_waiter);
        public: virtual ~PromiseReturnHandler();

        public: virtual void handle(CryptoPP::ByteQueue& content);
        public: virtual void reject(std::__exception_ptr::exception_ptr exception);
    };
}

inline Elonef::PromiseReturnHandler::~PromiseReturnHandler() {
    //std::cout << "del " << this << std::endl << std::flush;
    //sleep(1);
}

inline Elonef::PromiseReturnHandler::PromiseReturnHandler(std::shared_ptr<Elonef::DataWaiter<CryptoPP::ByteQueue>> data_waiter) {
    // std::cout << "mke: " << this << std::endl;
    this->data = data_waiter;
}

inline void Elonef::PromiseReturnHandler::handle(CryptoPP::ByteQueue& content) {
    this->data->set_value(content);
}

inline void Elonef::PromiseReturnHandler::reject(std::__exception_ptr::exception_ptr err) {
    data->reject(err);
}