#pragma once

#include <iostream>
#include <cryptopp/queue.h>
#include <vector>
#include <cryptopp/rsa.h>
#include "types.hpp"

std::ostream &operator<<(std::ostream &ostr, const CryptoPP::ByteQueue val);
std::ostream &operator<<(std::ostream &ostr, const CryptoPP::SecByteBlock val);
std::ostream &operator<<(std::ostream& ostr, const CryptoPP::RSA::PublicKey key);
std::ostream &operator<<(std::ostream& ostr, const Elonef::ECDSA::PublicKey key);
template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &ostr, const std::pair<T1, T2> val);
template<typename T>
std::ostream &operator<<(std::ostream& ostr, const std::vector<T>& vec);
template<typename T>
std::ostream &operator<<(std::ostream& ostr, const std::list<T>& vec);
template<typename Key, typename T>
std::ostream &operator<<(std::ostream& ostr, const std::unordered_map<Key, T>& map);


template<typename T1, typename T2>
inline std::ostream &operator<<(std::ostream &ostr, const std::pair<T1, T2> val) {
    return (ostr << "[" << val.first << ", " << val.second << "]");
}

template<typename T>
inline std::ostream &operator<<(std::ostream& ostr, const std::vector<T>& vec) {
    ostr << "[";
    for (size_t idx=0; idx<vec.size(); idx++) {
        if (idx!=0) {
            ostr << ", ";
        }
        ostr << vec[idx];
    }
    ostr << "]";
    return ostr;
}

template<typename T>
std::ostream &operator<<(std::ostream& ostr, const std::list<T>& lis) {
    std::vector<T> vec(lis.begin(), lis.end());
    return (ostr << vec);
}

template<typename Key, typename T>
std::ostream &operator<<(std::ostream& ostr, const std::unordered_map<Key, T>& map) {
    ostr << "{";
    for (const auto& element: map) {
        if (element!=*map.begin()) {
            ostr << ", ";
        }
        ostr << element.first << ": " << element.second;
    }
    ostr << "}";
    return ostr;
}
