#pragma once

#include <cryptopp/queue.h>

#define ELONEF_UUID_SIZE 16

namespace Elonef {
    CryptoPP::ByteQueue uuid();
}