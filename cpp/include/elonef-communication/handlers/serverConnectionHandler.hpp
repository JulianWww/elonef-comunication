#pragma once

#include <elonef-communication/handlers/connectionHandler.hpp>
#include <elonef-communication/types.hpp>
#include <elonef-communication/handlers/messageHandler.hpp>
#include "../keys/genKeys.hpp"

#define ELONEF_SERVER_TYPE websocket::WSServer<Elonef::ServerConnectionHandler, Elonef::ServerConnectionData>

namespace Elonef {
    struct ServerConnectionData {
        bool authenticated = false;
        CryptoPP::ByteQueue auth_data;
        std::string uid;
    };

    class ServerConnectionHandler : public ConnectionHandler, public MessageHandler<ELONEF_SERVER_TYPE::Connection, ServerConnectionHandler, ServerConnectionData> {
        private: ELONEF_SERVER_TYPE server;

        public: std::function<PublicClientKey*(std::string id)> get_public_key;

        public: ServerConnectionHandler(std::string ip, uint16_t port,
            std::function<PublicClientKey*(std::string id)> get_public_key
        );
        public: ~ServerConnectionHandler();

        // called before a connection is opend.
        public: bool onWSConnect(ELONEF_SERVER_TYPE::Connection& conn, const char* request_uri, const char* host, const char* origin, const char* protocol,
                   const char* extensions, char* resp_protocol, uint32_t resp_protocol_size, char* resp_extensions,
                   uint32_t resp_extensions_size);
        // callded after a connection is fully established.
        public: void onWSConnectionEstablished(ELONEF_SERVER_TYPE::Connection& conn);
        // called when a new messge is recived
        public: void onWSMsg(ELONEF_SERVER_TYPE::Connection& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len);
        // called when a ws socket is closed
        public: void onWSClose(ELONEF_SERVER_TYPE::Connection& conn, uint16_t status_code, const char* reason);

        public: void handle_auth(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ServerConnectionData& connData);
        public: CryptoPP::ByteQueue custom_handler(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ServerConnectionData& connData);

        private: CryptoPP::ByteQueue get_public_key_encoder(CryptoPP::ByteQueue& queue, std::function<SignedKey*(PublicClientKey* key)> key_getter);

        private: CryptoPP::ByteQueue get_data_key(CryptoPP::ByteQueue& queue);
        private: CryptoPP::ByteQueue get_signature_key(CryptoPP::ByteQueue& queue);

        public: virtual void run_blocking();
    };
};