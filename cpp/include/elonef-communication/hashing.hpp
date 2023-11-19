#pragma once

#include <functional>
#include <string>
#include "utils.hpp"

namespace std {
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
}

template<>
struct std::hash<CryptoPP::ByteQueue> {
    inline size_t operator() (const CryptoPP::ByteQueue& que) const {
        std::string str = Elonef::toString(que);
        std::hash<std::string> hasher;
        return hasher(str);
    }
};

template<typename T1, typename T2>
struct std::hash<std::pair<T1, T2>> {
    inline std::size_t operator () (const std::pair<T1, T2> &p) const {
        size_t seed = 0;
        hash_combine(seed, p.first);
        hash_combine(seed, p.second);
        return seed;  
    }
};