#pragma once

#include <future>
#include <mutex>

namespace Elonef {
    template<typename T>
    class DataWaiter {
        protected: std::promise<T> promise;
        private: std::mutex mu;
        public: std::future<T> future;

        public: DataWaiter();
        
        public: T get();
        public: void wait();
        public: void set_value(const T&& value);
        public: void set_value(T&& value);
        public: void set_value(T& value);
        public: void set_value();
        public: void reject(std::string reason);

    };
}

template<typename T>
inline Elonef::DataWaiter<T>::DataWaiter() : promise(), future(promise.get_future()) {}

template<typename T>
inline T Elonef::DataWaiter<T>::get() {
    return this->future.get();
}

template<typename T>
inline void Elonef::DataWaiter<T>::wait() {
    this->future.wait();
}

template<typename T>
inline void Elonef::DataWaiter<T>::set_value(const T&& value) {
    //this->mu.lock();
    this->promise.set_value(value);
    //this->mu.unlock();
}

template<typename T>
inline void Elonef::DataWaiter<T>::set_value(T&& value) {
    //this->mu.lock();
    this->promise.set_value(value);
    //this->mu.unlock();
}

template<typename T>
inline void Elonef::DataWaiter<T>::set_value(T& value) {
    //this->mu.lock();
    this->promise.set_value(value);
    //this->mu.unlock();
}

template<typename T>
inline void Elonef::DataWaiter<T>::set_value() {
    //this->mu.lock();
    this->promise.set_value();
    //this->mu.unlock();
}

template<typename T>
inline void Elonef::DataWaiter<T>::reject(std::string reason) {
    std::runtime_error* err = new std::runtime_error(reason);
    //this->mu.lock();
    this->promise.set_exception(err);
    //this->mu.unlock();
}