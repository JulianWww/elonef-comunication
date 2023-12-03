#pragma once

#include <unordered_map>
#include <functional>
#include <list>
#include "handlers/messageHandler.hpp"
#include "handlers/return_handlers/callback_return_handler.hpp"
#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include "cache_data.hpp"
#include "data_waiter.hpp"

namespace Elonef {
    template<typename Key, typename T, typename Hash=std::hash<Key>>
    class Cache {
        private: std::mutex mu;
        private: std::unordered_map<Key, std::unique_ptr<Elonef::DataWaiter<T>>, Hash> cache;
        private: std::function<CryptoPP::ByteQueue(const std::list<Key>& keys)> encoder;

        private: const CryptoPP::byte fetcher;

        public: Cache(std::function<CryptoPP::ByteQueue(const std::list<Key>& keys)> encoder, CryptoPP::byte fetcher);


        public: template<typename Handler>
        void ensure_presance(const std::vector<Key>& key, ix::WebSocket& ws, Handler* handler, std::function<std::vector<std::pair<Key, T>>(Handler* handler, CryptoPP::ByteQueue& queue)> decoder);
        public: Elonef::DataWaiter<T>* get(const Key& key);
        
        public: template<typename Handler>
        static void handle(Cache<Key, T, Hash>* _this, CacheHandlerData<Key, Handler, T>* ptr, CryptoPP::ByteQueue& content);

        public: std::string print();


        private: template<typename Handler>
        static void deleter(Cache<Key, T, Hash>* cache, CacheHandlerData<Key, Handler, T>* data);
    };
}

template<typename Key, typename T, typename Hash>
inline Elonef::Cache<Key, T, Hash>::Cache(
    std::function<CryptoPP::ByteQueue(const std::list<Key>& keys)> _encoder, 
    CryptoPP::byte _fetcher) : 
        encoder(_encoder), fetcher(_fetcher) {}

template<typename Key, typename T, typename Hash>
template<typename Handler>
inline void Elonef::Cache<Key, T, Hash>::ensure_presance(const std::vector<Key>& key, ix::WebSocket& ws, Handler* handler, std::function<std::vector<std::pair<Key, T>>(Handler* handler, CryptoPP::ByteQueue& queue)> decoder) {
    std::list<Key> missing;

    this->mu.lock();
    for (const Key& user : key) {
        if (!this->cache.contains(user)) {
            this->cache.insert({user, std::make_unique<DataWaiter<T>>()});
            missing.push_back(user);
        };
    }
    this->mu.unlock();
    if (missing.size() == 0) {
        return;
    }

    CryptoPP::ByteQueue queue = this->encoder(missing);

    CacheHandlerData<Key, Handler, T>* data = new CacheHandlerData<Key, Handler, T>(key, handler, decoder);
    handler->send(ws, queue, this->fetcher, 
        new CallbackReturnHandler<Cache<Key, T, Hash>, CacheHandlerData<Key, Handler, T>> (&Cache<Key, T, Hash>::handle<Handler>, this, data, &Cache<Key, T, Hash>::deleter<Handler>));
}

template<typename Key, typename T, typename Hash>
inline Elonef::DataWaiter<T>* Elonef::Cache<Key, T, Hash>::get(const Key& key) {
    return this->cache[key].get();;
}

template<typename Key, typename T, typename Hash>
template<typename Handler>
inline void Elonef::Cache<Key, T, Hash>::handle(Cache<Key, T, Hash>* _this, CacheHandlerData<Key, Handler, T>* data, CryptoPP::ByteQueue& content) {
    std::vector<std::pair<Key, T>> keys = data->decode(content);

    _this->mu.lock();
    for (std::pair<Key, T>& key : keys) {
        _this->cache[key.first]->set_value(key.second);
        data->users.erase(std::find(data->users.begin(), data->users.end(), key.first));
    }

    for (Key& user : data->users) {
        //_this->cache[user].set_value();
        _this->cache.erase(user);
    }
    _this->mu.unlock();

    delete data;
}

template<typename Key, typename T, typename Hash>
std::string Elonef::Cache<Key, T, Hash>::print() {
    std::string out;
    for (auto& element : this->cache) {
        out = out + element.first + "\t";
        if (check_if_future_is_ready<T>(element.second->future)) {
            out = out + "true";
        }
        else {
            out = out + "false";
        }
        out = out + "\n";
    }
    return out;
}


template<typename Key, typename T, typename Hash>
template<typename Handler>
void Elonef::Cache<Key, T, Hash>::deleter(Cache<Key, T, Hash>* cache, CacheHandlerData<Key, Handler, T>* data) {
    delete data;
}