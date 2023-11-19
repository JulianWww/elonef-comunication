#pragma once

#include <elonef-communication/handlers/connectionHandler.hpp>
#include <elonef-communication/types.hpp>
#include <elonef-communication/handlers/messageHandler.hpp>
#include "../keys/genKeys.hpp"
#include "../sliding_time_window.hpp"

#define ELONEF_SERVER_TYPE websocket::WSServer<Elonef::ServerConnectionHandler, Elonef::ServerConnectionData>

namespace Elonef {
    struct ServerConnectionData {
        bool authenticated = false;
        CryptoPP::ByteQueue auth_data;
        std::string uid;
    };

    class ServerConnectionHandler : public ConnectionHandler, public MessageHandler<ELONEF_SERVER_TYPE::Connection, ServerConnectionHandler, ServerConnectionData> {
        private: ELONEF_SERVER_TYPE server;

        private: std::function<const PublicClientKey*(const std::string& id, const std::string& userid)> get_public_key;
        private: std::function<const CryptoPP::ByteQueue*(const std::pair<std::string, CryptoPP::ByteQueue>& chat_key_id, const std::string& userid)> chat_key_fetcher;
        private: std::function<const std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*>(const std::string& chat_id, const std::string& userid)> newest_chat_key_fetcher;
        private: std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys)> chat_key_setter;
        private: std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& message)> message_setter;
        
        private: SlidingTimeWindow sendTimeUploadWindow;

        public: ServerConnectionHandler(std::string ip, uint16_t port, const SlidingTimeWindow window,
            std::function<const PublicClientKey*(const std::string& id, const std::string& userid)> get_public_key,
            std::function<const CryptoPP::ByteQueue*(const std::pair<std::string, CryptoPP::ByteQueue>& chat_key_id, const std::string& userid)> get_chat_key,
            std::function<const std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*>(const std::string& chat_id, const std::string& userid)> newest_chat_key_fetcher,
            std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys)> chat_key_setter,
            std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& message)> message_setter
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



        private: static const SignedKey* get_signed_key_for_user(ServerConnectionHandler* _this, const std::string& uid, const std::string& requester);
        private: static const SignedKey* get_data_key_for_user(ServerConnectionHandler* _this, const std::string& uid, const std::string& requester);
        private: static const CryptoPP::ByteQueue* get_chat_key_for_user(ServerConnectionHandler* _this, const std::pair<std::string, CryptoPP::ByteQueue>& key_id, const std::string& requester);

        private: static void auth(Elonef::ServerConnectionHandler* _this, ELONEF_SERVER_TYPE::Connection* conn, CryptoPP::ByteQueue& queue);

        private: template<typename Key, typename T>
        CryptoPP::ByteQueue buid_cache_request_return( 
            const std::vector<Key>& keys, 
            const ServerConnectionData& connData,
            std::function<const T*(ServerConnectionHandler* _this, const Key& key, const std::string& userid)> fetcher, 
            std::function<CryptoPP::ByteQueue(const T& value)> toBytes
        );

        private: CryptoPP::ByteQueue get_data_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: CryptoPP::ByteQueue get_signature_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: CryptoPP::ByteQueue get_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: CryptoPP::ByteQueue get_newest_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: void set_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: void add_message(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);

        private: bool verify_message_authenticity(CryptoPP::ByteQueue message, const std::string& userid);
        private: bool verify_message_signature(CryptoPP::ByteQueue& message, const std::string& userid);
        private: bool verify_send_time(CryptoPP::ByteQueue& message);

        public: virtual void run_blocking();
    };
};

template<typename Key, typename T>
CryptoPP::ByteQueue Elonef::ServerConnectionHandler::buid_cache_request_return(
        const std::vector<Key>& keys,
        const ServerConnectionData& connData,
        std::function<const T*(ServerConnectionHandler* _this, const Key& key, const std::string& userid)> fetcher, 
        std::function<CryptoPP::ByteQueue(const T& value)> _toBytes
    ) {
    std::list<std::pair<Key, CryptoPP::ByteQueue>> out;
    for (const Key& key: keys) {
        const T* value = fetcher(this, key, connData.uid);
        if (value == nullptr) {
            continue;
        }
        out.push_back({
            key, _toBytes(*value)
        });
    }
    return toBytes(out.begin(), out.end());
}
