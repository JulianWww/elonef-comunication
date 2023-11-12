#pragma once

#include <thread>
#include <mutex>

namespace Elonef {
    class ConnectionHandler {
        private: std::mutex stop_mutex;
        protected: bool running = true;
        protected: std::thread* wsthread = nullptr;

        public: ~ConnectionHandler();

        // stops the listending thread if one exists;
        public: void stop();
        // run the recv loop in a new thread
        public: void run();
        
        // run the recv loop in the current thread.
        public: virtual void run_blocking() = 0;
    };
}