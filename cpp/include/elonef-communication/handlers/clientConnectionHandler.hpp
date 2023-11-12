#pragma once

#include "connectionHandler.hpp"
#include "messageHandler.hpp"
#include "../types.hpp"
#include <condition_variable>
#include "../keys/genKeys.hpp"
#include "../data_waiter.hpp"
#include <cryptopp/rsa.h>
#include "client_data/client_auth_data.hpp"
#include "client_data/client_connection_data.hpp"
#include "../cache.hpp"


#define ELONEF_CLIENT_TYPE websocket::WSClient<Elonef::ClientConnectionHandler, Elonef::ClientConnectionData>

namespace Elonef {
    class ClientConnectionHandler : public ConnectionHandler, public MessageHandler<ELONEF_CLIENT_TYPE::Connection, ClientConnectionHandler, ClientConnectionData> {
        private: ELONEF_CLIENT_TYPE client;

        private: uint64_t timeout;
        private: const char* server_ip;
        private: uint16_t server_port;
        private: const char* request_uri;
        private: const char* host;
        private: const char* origin;
        private: const char* protocol;
        private: const char* extensions;
        private: char* resp_protocol;
        private: uint32_t resp_protocol_size;
        private: char* resp_extensions;
        private: uint32_t resp_extensions_size;
        
        private: std::unique_ptr<CryptoPP::RSA::PrivateKey> data_key;
        private: std::unique_ptr<Elonef::ECDSA::PrivateKey> sign_key;
        private: std::unique_ptr<std::string> user_id;

        private: Elonef::DataWaiter<void*> ready;

        private: Elonef::Cache<std::string, CryptoPP::RSA::PublicKey> data_key_cache;
        private: Elonef::Cache<std::string, Elonef::ECDSA::PublicKey> signed_key_cache;

        public: ClientConnectionHandler(
                uint64_t timeout, const char* server_ip, uint16_t server_port, const char* request_uri,
                const char* host, const char* origin = nullptr, const char* protocol = nullptr,
                const char* extensions = nullptr, char* resp_protocol = nullptr, uint32_t resp_protocol_size = 0,
                char* resp_extensions = nullptr, uint32_t resp_extensions_size = 0);

        public: bool connect();

        // called when a new messge is recived
        public: void onWSMsg(ELONEF_CLIENT_TYPE::Connection& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len);
        // called when a ws socket is closed
        public: void onWSClose(ELONEF_CLIENT_TYPE::Connection& conn, uint16_t status_code, const char* reason);

        void handle_auth(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ClientConnectionData& connData);
        CryptoPP::ByteQueue custom_handler(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ClientConnectionData& connData);
        // authenticate the connection make sure to call after connect
        void authenticate(const Elonef::PrivateClientKey& key);

        public: virtual void run_blocking();

        private: void wait_for_auth();

        public: void load_data_keys(const std::vector<std::string>& keys);
        public: void load_signature_keys(const std::vector<std::string>& keys);
    };
    
}