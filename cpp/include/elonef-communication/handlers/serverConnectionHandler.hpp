#pragma once

#include <elonef-communication/handlers/connectionHandler.hpp>
#include <elonef-communication/types.hpp>
#include <thread>

#define ELONEF_SERVER_TYPE websocket::WSServer<Elonef::ServerConnectionHandler, Elonef::ServerConnectionData>

namespace Elonef {
    struct ServerConnectionData {

    };

    class ServerConnectionHandler : public ConnectionHandler {
        private: ELONEF_SERVER_TYPE server;
        private: bool running = true;
        private: std::thread wsthread;

        public: ServerConnectionHandler(std::string ip, uint16_t port);
        public: ~ServerConnectionHandler();

        //public: void on_connection(std::string ip, uint16_t port);
        public: bool onWSConnect(ELONEF_SERVER_TYPE::Connection& conn, const char* request_uri, const char* host, const char* origin, const char* protocol,
                   const char* extensions, char* resp_protocol, uint32_t resp_protocol_size, char* resp_extensions,
                   uint32_t resp_extensions_size);
        public: void onWSConnectionEstablished(ELONEF_SERVER_TYPE::Connection& conn);
        public: void onWSMsg(ELONEF_SERVER_TYPE::Connection& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len);
        public: void onWSClose(ELONEF_SERVER_TYPE::Connection& conn, uint16_t status_code, const char* reason);

        private: void send(ELONEF_SERVER_TYPE::Connection& conn, CryptoPP::ByteQueue& queue);

        public: void run();
        public: void stop();
        
        public: void run_blocking();
    };
};