#pragma once

#include <cryptopp/eccrypto.h>
#include <websocket.h>
#include "handlers/serverConnectionHandler.hpp"

namespace Elonef {
    namespace ECDSA {
        typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey PrivateKey;
        typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey PublicKey;
        typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer Signer;
        typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier Verifier;
    }
}