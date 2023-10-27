#pragma once

#include <string>

namespace Elonef {
    struct KeyPair {
        std::string publicKey;
        std::string privateKey;
    };
    

    KeyPair generateKeyPairRSA();
    KeyPair generateKeyPairECDSA();
}