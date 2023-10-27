#pragma once

#include <cryptopp/queue.h>

namespace Elonef {
    CryptoPP::ByteQueue sign_nonstreamable(CryptoPP::ByteQueue data, CryptoPP::ByteQueue key);
}