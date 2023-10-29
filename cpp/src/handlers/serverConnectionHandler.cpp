#include <elonef-communication/handlers/serverConnectionHandler.hpp>
#include <elonef-communication/utils.hpp>
#include <iostream>

Elonef::ServerConnectionHandler::ServerConnectionHandler(std::string ip, uint16_t port) {
    if (!this->server.init(ip.data(), port)) {
      std::cout << "wsserver init failed: " << this->server.getLastError() << std::endl;
      return;
    }
}

Elonef::ServerConnectionHandler::~ServerConnectionHandler() {
    this->stop();
}

bool Elonef::ServerConnectionHandler::onWSConnect(
    ELONEF_SERVER_TYPE::Connection& conn, 
    const char* request_uri, 
    const char* host, 
    const char* origin, 
    const char* protocol,
    const char* extensions, 
    char* resp_protocol, 
    uint32_t resp_protocol_size, 
    char* resp_extensions,
    uint32_t resp_extensions_size) {
    
    std::cout << "started connection " << conn.isConnected() << std::endl;
    return true;
}

void Elonef::ServerConnectionHandler::onWSConnectionEstablished(ELONEF_SERVER_TYPE::Connection& conn) {
    auto data = Elonef::toQueue("test message");
    this->send(conn, data);
}

void Elonef::ServerConnectionHandler::onWSMsg(ELONEF_SERVER_TYPE::Connection& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len){
    std::cout << (char*) payload << std::endl;
    std::cout << "got message" << std::endl;
}

void Elonef::ServerConnectionHandler::onWSClose(ELONEF_SERVER_TYPE::Connection& conn, uint16_t status_code, const char* reason){
    std::cout << "connection closed" << std::endl;
}

void Elonef::ServerConnectionHandler::send(ELONEF_SERVER_TYPE::Connection& conn, CryptoPP::ByteQueue& queue) {
    const CryptoPP::SecByteBlock data = Elonef::toSecBlock(queue);
    conn.send(websocket::OPCODE_BINARY, data.data(), data.SizeInBytes());
}

void run_server(Elonef::ServerConnectionHandler* handler) {
    handler->run_blocking();
}

void Elonef::ServerConnectionHandler::run () {
    this->wsthread = std::thread(run_server, this);
}

void Elonef::ServerConnectionHandler::stop() {
    if (this->running) {
        this->running = false;
        this->wsthread.join();
    }
}

void Elonef::ServerConnectionHandler::run_blocking() {
    while (this->running) {
        this->server.poll(this);
        std::this_thread::yield();
    }
}