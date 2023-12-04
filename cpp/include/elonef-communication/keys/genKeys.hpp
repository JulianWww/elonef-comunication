#pragma once

#include <string>
#include <unordered_map>

namespace Elonef {
    struct KeyPair {
        std::string publicKey;
        std::string privateKey;
    };

    struct SignedKey {
        std::string key;
        std::unordered_map<std::string, std::string> signatures;

        std::string print(size_t indent=0);
    };

    struct PublicClientKey {
        SignedKey data_key;
        SignedKey sign_key;
        std::string id;

        std::string print(size_t indent=0);
    };

    struct PrivateClientKey {
        std::string sign_key;
        std::string data_key;
        std::string server_key;
        std::unordered_map<std::string, std::string> ca_keys;
        std::string uid; 
    };

    struct ClientKeys {
        PrivateClientKey private_key;
        PublicClientKey public_key;
    };
    

    KeyPair generateKeyPairRSA();
    KeyPair generateKeyPairECDSA();

    ClientKeys generateClientKeys(std::string server_key, std::unordered_map<std::string, std::string> ca_keys, std::string id);
}