#pragma once

#include <vector>
#include <string>

namespace Elonef {
    template<typename Key, typename Handler, typename T>
    struct CacheHandlerData {
        std::unordered_set<Key> users;
        Handler* handler;
        std::function<std::vector<std::pair<Key, T>>(Handler* handler, CryptoPP::ByteQueue& queue)> decoder;

        CacheHandlerData(const std::unordered_set<Key> users, Handler* handler, std::function<std::vector<std::pair<Key, T>>(Handler* handler, CryptoPP::ByteQueue& queue)>& decoder);

        std::vector<std::pair<Key, T>> decode(CryptoPP::ByteQueue& queue);
    };
}

template<typename Key, typename Handler, typename T>
inline Elonef::CacheHandlerData<Key, Handler, T>::CacheHandlerData(const std::unordered_set<Key> _users, Handler* _handler, std::function<std::vector<std::pair<Key, T>>(Handler* handler, CryptoPP::ByteQueue& queue)>& _decoder) : 
        users(_users), handler(_handler), decoder(_decoder) {
}

template<typename Key, typename Handler, typename T>
inline std::vector<std::pair<Key, T>> Elonef::CacheHandlerData<Key, Handler, T>::decode(CryptoPP::ByteQueue& queue) {
    return this->decoder(this->handler, queue);
}