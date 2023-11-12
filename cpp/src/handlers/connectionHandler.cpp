#include <elonef-communication/handlers/connectionHandler.hpp>
#include <iostream>

Elonef::ConnectionHandler::~ConnectionHandler() {
    this->stop();
    if (this->wsthread != nullptr) {
        delete this->wsthread;
    }
}

void Elonef::ConnectionHandler::stop() {
    // use mutex to avoid deadlock in join
    // handle case where no thread has been created
    this->stop_mutex.lock();
    if (this->wsthread == nullptr) {
        return;
    }

    if (this->wsthread->joinable()){
        this->running = false;
        if (this->wsthread->get_id() != std::this_thread::get_id()) {
            this->wsthread->join();
        }
    }
    this->stop_mutex.unlock();
}

void run_runMethod(Elonef::ConnectionHandler* handler) {
    handler->run_blocking();
}

void Elonef::ConnectionHandler::run () {
    this->wsthread = new std::thread(run_runMethod, this);
}