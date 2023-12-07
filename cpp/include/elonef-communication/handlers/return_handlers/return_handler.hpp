#pragma once

#include <cryptopp/queue.h>
#include <iostream>

namespace Elonef {
    class ReturnHandler {
        public: virtual void handle(CryptoPP::ByteQueue& content)=0;
        public: virtual void reject(std::__exception_ptr::exception_ptr exception)=0;
        
        public: virtual ~ReturnHandler() = default;

        public: template<typename Error>
        void reject(Error exception);
    };
}

template<typename Error>
inline void Elonef::ReturnHandler::reject(Error exception) {
    try {
        throw exception;
    }
    catch (...) {
        this->reject(std::current_exception());
    }
}