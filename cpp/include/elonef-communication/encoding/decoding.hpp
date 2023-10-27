#pragma once

#include <cryptopp/queue.h>

namespace Elonef {
    size_t toSize_T(CryptoPP::ByteQueue& que);
    size_t short_toSize_T(CryptoPP::ByteQueue& que);
    std::string toDynamicSizeString(CryptoPP::ByteQueue& que, size_t(*toNum)(CryptoPP::ByteQueue&) = toSize_T);
    CryptoPP::ByteQueue extractDynamicLengthQueue(CryptoPP::ByteQueue& queue, size_t(*toNum)(CryptoPP::ByteQueue&) = toSize_T);

    template <typename Iter, typename T>
    Iter toIterable(CryptoPP::ByteQueue& que, T(*extractor)(CryptoPP::ByteQueue& que), Iter(*iter_builder)(size_t que)) {
        size_t length = toSize_T(que);
        Iter out = iter_builder(length);
        for (auto start=out.begin(); start!=out.end(); start++) {
            *start = extractor(que);
        }
        return out;
    }

    template<typename T>
    std::vector<T> make_vector(size_t size) {
        return std::vector<T>(size);
    }

    template<typename T>
    std::list<T> make_list(size_t size) {
        return std::list<T>(size);
    }
}