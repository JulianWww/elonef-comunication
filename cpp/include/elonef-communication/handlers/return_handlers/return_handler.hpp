#pragma once

#include <cryptopp/queue.h>

namespace Elonef {
    class ReturnHandler {
        public: virtual void handle(CryptoPP::ByteQueue& content)=0;
    };
}