#pragma once

#include <future>
#include <mutex>
#include "print.hpp"

namespace Elonef {
    template<typename T>
    class DataWaiter {
        protected: std::promise<T> promise;
        private: std::mutex mu;
        private: T data;
        private: bool resolved=false;
        public: std::future<T> future;

        public: DataWaiter();
        
        public: T get();
        public: void wait();
        public: void set_value(const T&& value);
        public: void set_value(T&& value);
        public: void set_value(T& value);
        public: void set_value();
        public: void reject(std::string reason);
        public: void reject(std::__exception_ptr::exception_ptr error);

    };
}

template<typename T>
inline Elonef::DataWaiter<T>::DataWaiter() : promise(), future(promise.get_future()) {}

template<typename T>
inline T Elonef::DataWaiter<T>::get() {
    this->mu.lock();
    if (!this->resolved) {
        this->data = this->future.get();
        this->resolved = true;
    }
    this->mu.unlock();
    return this->data;
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
    try {
        throw new std::runtime_error(reason);
    }
    catch(...) {
        this->reject(std::current_exception());
    }
}

template<typename T>
inline void Elonef::DataWaiter<T>::reject(std::__exception_ptr::exception_ptr error) {
    this->promise.set_exception(error);
}