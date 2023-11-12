#pragma once

#include <future>

namespace Elonef {
    template<typename T>
    class DataWaiter {
        protected: std::promise<T> promise;
        public: std::future<T> future;

        public: DataWaiter();
        
        public: T get();
        public: void wait();
        public: void set_value(const T&& value);
        public: void set_value(T&& value);
        public: void set_value(T& value);
        public: void set_value();

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
    return this->promise.set_value(value);
}

template<typename T>
inline void Elonef::DataWaiter<T>::set_value(T&& value) {
    return this->promise.set_value(value);
}

template<typename T>
inline void Elonef::DataWaiter<T>::set_value(T& value) {
    return this->promise.set_value(value);
}

template<typename T>
inline void Elonef::DataWaiter<T>::set_value() {
    return this->promise.set_value();
}