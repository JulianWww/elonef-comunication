#pragma once
#include <cryptopp/queue.h>
#include <cryptopp/rsa.h>

namespace Elonef
{
    CryptoPP::ByteQueue encript_rsa(CryptoPP::ByteQueue toEncrypt, CryptoPP::RSA::PublicKey key);
    CryptoPP::ByteQueue decript_rsa(CryptoPP::ByteQueue toDecrypt, CryptoPP::RSA::PrivateKey key);
} // namespace Elonef