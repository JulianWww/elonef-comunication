#pragma once
#include <iostream>
#include <cryptopp/queue.h>
#include <cryptopp/rsa.h>
#include "handlers/client_data/client_auth_data.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <unordered_set>

inline std::ostream& operator<<(std::ostream& out, CryptoPP::ByteQueue queu) {
    return (out << Elonef::toBase64(queu));
}
inline std::ostream& operator<<(std::ostream& out, Elonef::ClientAuthData queu) {
    return out;
}
inline std::ostream& operator<<(std::ostream& out, CryptoPP::RSA::PublicKey queu) {
    return out;
}
inline std::ostream& operator<<(std::ostream& out, Elonef::ECDSA::PublicKey queu) {
    CryptoPP::ByteQueue o;
    queu.Save(o);
    return (out << o);
}

template<typename T1, typename T2>
inline std::ostream& operator<<(std::ostream& out, std::pair<T1, T2> queu) {
    return (out << queu.first << ", " << queu.second);
}

template<typename T1>
inline std::ostream& operator<<(std::ostream& out, std::vector<T1> queu) {
    for (T1& val : queu) {
        out << val << ", ";
    }
    return out;
}

template<typename T1>
inline std::ostream& operator<<(std::ostream& out, std::unordered_set<T1> queu) {
    for (auto i=queu.begin(); i!=queu.end(); i++) {
        out << *i << ", ";
    }
    return out;
}