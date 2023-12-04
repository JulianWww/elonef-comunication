#pragma once

#include <string>
#include <unordered_map>
#include <cryptopp/queue.h>

namespace Elonef {
    struct KeyPair {
        std::string publicKey;
        std::string privateKey;
    };

    struct SignedKey {
        SignedKey(CryptoPP::ByteQueue& data);
        SignedKey();

        std::string key;
        std::unordered_map<std::string, std::string> signatures;

        CryptoPP::ByteQueue toQueue();
    };

    struct PublicClientKey {
        PublicClientKey(CryptoPP::ByteQueue& data);
        PublicClientKey();

        SignedKey data_key;
        SignedKey sign_key;
        std::string id;

        CryptoPP::ByteQueue toQueue();
    };

    struct PrivateClientKey {
        PrivateClientKey(CryptoPP::ByteQueue& data);
        PrivateClientKey();

        std::string sign_key;
        std::string data_key;
        std::string server_key;
        std::unordered_map<std::string, std::string> ca_keys;
        std::string uid;

        CryptoPP::ByteQueue toQueue();
    };

    struct ClientKeys {
        PrivateClientKey private_key;
        PublicClientKey public_key;
    };
    

    KeyPair generateKeyPairRSA();
    KeyPair generateKeyPairECDSA();

    ClientKeys generateClientKeys(std::string server_key, std::unordered_map<std::string, std::string> ca_keys, std::string id);
}