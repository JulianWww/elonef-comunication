#pragma once

#include <unordered_map>
#include <functional>
#include "handlers/messageHandler.hpp"
#include "handlers/return_handlers/callback_return_handler.hpp"


namespace Elonef {
    template<typename Key, typename T>
    class Cache {
        private: std::unordered_map<Key, Elonef::DataWaiter<T>> cache;
        private: std::function<std::vector<std::pair<Key, T>>(CryptoPP::ByteQueue& queue)> decoder;
        private: std::function<CryptoPP::ByteQueue(const std::vector<Key>& keys)> encoder;

        public: Cache(std::function<std::vector<std::pair<Key, T>>(CryptoPP::ByteQueue& queue)> decoder, std::function<CryptoPP::ByteQueue(const std::vector<Key>& keys)> encoder);


        public: template<typename WS, typename Handler>
        void ensure_presance(const std::vector<Key>& key, WS& ws, Handler* handler);
        public: Elonef::DataWaiter<T>* get(const Key& key);
        
        public: template<typename WS>
        static void handle(Cache<Key, T>* _this, std::vector<Key>* ptr, CryptoPP::ByteQueue& content);

        public: std::string print();
    };
}

template<typename Key, typename T>
inline Elonef::Cache<Key, T>::Cache(std::function<std::vector<std::pair<Key, T>>(CryptoPP::ByteQueue& queue)> _decoder, std::function<CryptoPP::ByteQueue(const std::vector<Key>& keys)> _encoder) : 
        encoder(_encoder), decoder(_decoder) {}

template<typename Key, typename T>
template<typename WS, typename Handler>
inline void Elonef::Cache<Key, T>::ensure_presance(const std::vector<Key>& key, WS& ws, Handler* handler) {
    CryptoPP::ByteQueue queue = this->encoder(key);

    for (const Key& user : key) {
        this->cache.insert({user, DataWaiter<T>()});
    }

    auto callback = &Cache<Key, T>::handle<WS>;
    handler->send(ws, queue, 0x10, new CallbackReturnHandler<Cache<Key, T>, std::vector<Key>>(callback, this, new std::vector<std::string>(key)));
}

template<typename Key, typename T>
inline Elonef::DataWaiter<T>* Elonef::Cache<Key, T>::get(const Key& key) {
    return this->cache[key];
}

template<typename Key, typename T>
template<typename WS>
inline void Elonef::Cache<Key, T>::handle(Cache<Key, T>* _this, std::vector<Key>* users, CryptoPP::ByteQueue& content) {
    std::vector<std::pair<Key, T>> keys = _this->decoder(content);
    std::cout << keys.size() << std::endl;

    for (std::pair<Key, T>& key : keys) {
        _this->cache[key.first].set_value(key.second);
        users->erase(std::find(users->begin(), users->end(), key.first));
    }

    for (std::string& user : *users) {
        //_this->cache[user].set_value();
        _this->cache.erase(user);
    }

    delete users;
}

template<typename Key, typename T>
std::string Elonef::Cache<Key, T>::print() {
    std::string out;
    for (auto& element : this->cache) {
        out = out + element.first + "\t";
        if (check_if_future_is_ready<CryptoPP::RSA::PublicKey>(element.second.future)) {
            out = out + "true";
        }
        else {
            out = out + "false";
        }
        out = out + "\n";
    }
    return out;
}