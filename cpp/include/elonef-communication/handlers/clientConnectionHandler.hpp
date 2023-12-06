#pragma once

#include <cryptopp/rsa.h>
#include <condition_variable>
#include "messageHandler.hpp"
#include "client_data/client_connection_data.hpp"
#include "client_data/client_auth_data.hpp"
#include "../types.hpp"
#include "../keys/genKeys.hpp"
#include "../hashing.hpp"
#include "../data_waiter.hpp"
#include "../cache.hpp"


namespace Elonef {
    class ClientConnectionHandler : public MessageHandler<ClientConnectionHandler, ClientConnectionData> {
        friend MessageHandler<ClientConnectionHandler, ClientConnectionData>;
        friend ix::WebSocket;
        
        private: ix::WebSocket client;
        
        private: std::unique_ptr<CryptoPP::RSA::PrivateKey> data_key;
        private: std::unique_ptr<Elonef::ECDSA::PrivateKey> sign_key;
        private: std::unique_ptr<std::string> user_id;

        private: Elonef::DataWaiter<void*> ready;

        private: Elonef::Cache<std::string, CryptoPP::RSA::PublicKey> data_key_cache;
        private: Elonef::Cache<std::string, Elonef::ECDSA::PublicKey> signed_key_cache;
        private: Elonef::Cache<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue, std::hash<std::pair<std::string, CryptoPP::ByteQueue>>> chat_key_cache;

        public: ClientConnectionHandler(const char* request_uri);

        public: void connect();

        private: void onOpen(ix::WebSocket& conn);
        private: void onClose(ix::WebSocket& conn);

        private: void handle_auth(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ClientConnectionData& connData);
        private: CryptoPP::ByteQueue custom_handler(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ClientConnectionData& connData);
        // authenticate the connection make sure to call after connect
        public: void authenticate(const Elonef::PrivateClientKey& key);

        public: void start();
        public: void stop();

        private: void wait_for_auth();

        public: void load_data_keys(const std::unordered_set<std::string>& keys);
        public: void load_signature_keys(const std::unordered_set<std::string>& keys);
        public: void load_chat_keys(const std::string& chat_key, const CryptoPP::ByteQueue& key_id);
        public: void load_chat_keys(const std::pair<std::string, CryptoPP::ByteQueue>& key_id);
        public: void load_chat_keys(const std::unordered_set<std::pair<std::string, CryptoPP::ByteQueue>>& key_ids);
        public: std::pair<CryptoPP::ByteQueue, CryptoPP::ByteQueue> get_newest_chat_key(const std::string& str);
        public: void generate_chat_key(const std::unordered_set<std::string>& users, const std::string& chat_id);
        public: void send_message(CryptoPP::ByteQueue& message, const CryptoPP::byte message_type, const std::string& chat_id);
        public: std::vector<Elonef::Message> read_messages(const std::string& chat_id, const size_t& msg_id, const size_t& amount_of_messages);
        public: void read_messages(const std::string& chat_id, const size_t& msg_id, const size_t& amount_of_messages, ReturnHandler* handler);

        public: CryptoPP::ByteQueue make_api_request(const std::string& chat_id, CryptoPP::ByteQueue& data, const bool& wait_for_auth);
        public: void make_api_request(const std::string& call_id, CryptoPP::ByteQueue& data, const bool& wait_for_auth, ReturnHandler* handler);


        public: std::shared_ptr<Elonef::DataWaiter<CryptoPP::RSA::PublicKey>> get_data_key(const std::string& user);
        public: std::shared_ptr<Elonef::DataWaiter<Elonef::ECDSA::PublicKey>> get_signature_key(const std::string& user);
        public: std::shared_ptr<Elonef::DataWaiter<CryptoPP::ByteQueue>> get_chat_key(const std::string&, const CryptoPP::ByteQueue& key_id);
        public: std::shared_ptr<Elonef::DataWaiter<CryptoPP::ByteQueue>> get_chat_key(const std::pair<std::string, CryptoPP::ByteQueue>& id);


        public: std::vector<Message> decode_message(CryptoPP::ByteQueue& queue, const std::string& chat_id);
        private: CryptoPP::ByteQueue decrypt_chat_key(CryptoPP::ByteQueue& queue);


        private: static std::vector<std::pair<std::string, CryptoPP::RSA::PublicKey>> toRsaPublicVector(ClientConnectionHandler* _this, CryptoPP::ByteQueue& queue);
        private: static std::vector<std::pair<std::string, Elonef::ECDSA::PublicKey>> toECDSAPublicVector(ClientConnectionHandler* _this, CryptoPP::ByteQueue& queue);
        private: static std::vector<std::pair<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue>> toChatKeyVector(ClientConnectionHandler* _this, CryptoPP::ByteQueue& queue);
        private: static bool is_authenticated(ClientConnectionData& data);
    };
    
}