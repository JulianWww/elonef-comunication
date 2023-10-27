#pragma once

#include <crypto++/queue.h>

namespace Elonef {
    CryptoPP::ByteQueue encrypt(CryptoPP::ByteQueue to_encrypt, CryptoPP::SecByteBlock key);
    CryptoPP::ByteQueue decrypt(CryptoPP::ByteQueue to_decrypt, CryptoPP::SecByteBlock key);


    CryptoPP::SecByteBlock randomBytes(size_t size);
    CryptoPP::SecByteBlock randomKey();
}

