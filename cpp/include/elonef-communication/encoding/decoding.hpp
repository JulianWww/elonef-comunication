#pragma once

#include <cryptopp/queue.h>
#include <cryptopp/rsa.h>
#include "../types.hpp"

namespace Elonef {
    size_t toSize_T(CryptoPP::ByteQueue& que);
    size_t short_toSize_T(CryptoPP::ByteQueue& que);
    std::string toDynamicSizeString(CryptoPP::ByteQueue& que, size_t(*toNum)(CryptoPP::ByteQueue&) = toSize_T);
    CryptoPP::ByteQueue extractDynamicLengthQueue(CryptoPP::ByteQueue& queue, size_t(*toNum)(CryptoPP::ByteQueue&));
    CryptoPP::ByteQueue extractConstantLengthQueue(CryptoPP::ByteQueue& queue, size_t size);
    CryptoPP::byte extreactByte(CryptoPP::ByteQueue& queue);

    template <typename Iter, typename T>
    inline Iter toIterable(CryptoPP::ByteQueue& que, T(*extractor)(CryptoPP::ByteQueue& que), Iter(*iter_builder)(size_t que)) {
        size_t length = toSize_T(que);
        Iter out = iter_builder(length);
        for (auto start=out.begin(); start!=out.end(); start++) {
            *start = extractor(que);
        }
        return out;
    }

    template<typename Key, typename T>
    inline std::unordered_map<Key, T> toMap(CryptoPP::ByteQueue& que, Key(*keyExtractor)(CryptoPP::ByteQueue& que), T(*tExtractor)(CryptoPP::ByteQueue& que)) {
        size_t length = toSize_T(que);
        std::unordered_map<Key, T> out;
        for (size_t idx=0; idx<length; idx++) {
            out.insert({{keyExtractor(que), tExtractor(que)}});
        }
        return out;
    }

    template<typename T>
    inline std::vector<T> make_vector(size_t size) {
        return std::vector<T>(size);
    }

    template<typename T>
    inline std::list<T> make_list(size_t size) {
        return std::list<T>(size);
    }

    inline std::string toDynamicSizeString_long(CryptoPP::ByteQueue& que) {return toDynamicSizeString(que, toSize_T);};

    std::vector<std::string> toStringVector(CryptoPP::ByteQueue& queue);
    std::vector<std::pair<std::string, CryptoPP::RSA::PublicKey>> toRsaPublicVector(CryptoPP::ByteQueue& queue);
    std::vector<std::pair<std::string, Elonef::ECDSA::PublicKey>> toECDSAPublicVector(CryptoPP::ByteQueue& queue);
}