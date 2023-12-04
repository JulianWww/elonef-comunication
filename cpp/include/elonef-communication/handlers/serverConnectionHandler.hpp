#pragma once

#include <elonef-communication/types.hpp>
#include <elonef-communication/handlers/messageHandler.hpp>
#include <ixwebsocket/IXWebSocketServer.h>
#include "../sliding_time_window.hpp"
#include "../keys/genKeys.hpp"
#include <unordered_set>


namespace Elonef {
    struct ServerConnectionData {
        bool authenticated = false;
        CryptoPP::ByteQueue auth_data;
        std::string uid;
    };

    class ServerConnectionHandler : public MessageHandler<ServerConnectionHandler, ServerConnectionData>, public ix::WebSocketServer {
        friend MessageHandler<ServerConnectionHandler, ServerConnectionData>;

        public: typedef std::pair<std::vector<CryptoPP::ByteQueue>::iterator, std::vector<CryptoPP::ByteQueue>::iterator> VectorRange;
    
        private: std::function<PublicClientKey*(const std::string& id, const std::string& userid)> get_public_key;
        private: std::function<CryptoPP::ByteQueue*(const std::pair<std::string, CryptoPP::ByteQueue>& chat_key_id, const std::string& userid)> chat_key_fetcher;

        private: std::function<std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*>(const std::string& chat_id, const std::string& userid)> newest_chat_key_fetcher;
        private: std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys)> chat_key_setter;
        private: std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& message)> message_setter;
        private: std::function<VectorRange(const std::string& chat_id, const size_t& msg_idx, const size_t& count)> message_fetcher;
        
        private: SlidingTimeWindow sendTimeUploadWindow;

        public: ServerConnectionHandler(std::string ip, uint16_t port, const SlidingTimeWindow window,
            std::function<PublicClientKey*(const std::string& id, const std::string& userid)> get_public_key,
            std::function<CryptoPP::ByteQueue*(const std::pair<std::string, CryptoPP::ByteQueue>& chat_key_id, const std::string& userid)> get_chat_key,
            std::function<std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*>(const std::string& chat_id, const std::string& userid)> newest_chat_key_fetcher,
            std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys)> chat_key_setter,
            std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& message)> message_setter,
            std::function<VectorRange(const std::string& chat_id, const size_t& msg_idx, const size_t& count)> message_fetcher
        );
        

        // called before a connection is opend.
        private: bool onWSConnect(ix::WebSocket& conn, const char* request_uri, const char* host, const char* origin, const char* protocol,
                   const char* extensions, char* resp_protocol, uint32_t resp_protocol_size, char* resp_extensions,
                   uint32_t resp_extensions_size);
        
        private: void onOpen(ix::WebSocket& conn);
        private: void onClose(ix::WebSocket& conn);

        private: void handle_auth(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ServerConnectionData& connData);
        private: CryptoPP::ByteQueue custom_handler(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ServerConnectionData& connData);

        public: void make_api_request(const std::string& call_id, CryptoPP::ByteQueue& content, const std::unordered_set<std::string>& users, std::function<void(ServerConnectionHandler*, ix::WebSocket*, CryptoPP::ByteQueue&)> handler);



        private: static std::pair<PublicClientKey* ,SignedKey*> get_signed_key_for_user(ServerConnectionHandler* _this, const std::string& uid, const std::string& requester);
        private: static std::pair<PublicClientKey* ,SignedKey*> get_data_key_for_user(ServerConnectionHandler* _this, const std::string& uid, const std::string& requester);
        private: static std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*> get_chat_key_for_user(ServerConnectionHandler* _this, const std::pair<std::string, CryptoPP::ByteQueue>& key_id, const std::string& requester);

        private: static void auth(Elonef::ServerConnectionHandler* _this, ix::WebSocket* conn, CryptoPP::ByteQueue& queue);

        private: template<typename Key, typename T, typename Base>
        CryptoPP::ByteQueue buid_cache_request_return( 
            const std::vector<Key>& keys, 
            const ServerConnectionData& connData,
            std::function<std::pair<Base*, T*>(ServerConnectionHandler* _this, const Key& key, const std::string& userid)> fetcher, 
            std::function<CryptoPP::ByteQueue(const T& value)> toBytes
        );

        private: CryptoPP::ByteQueue get_data_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: CryptoPP::ByteQueue get_signature_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: CryptoPP::ByteQueue get_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: CryptoPP::ByteQueue get_newest_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: void set_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: void add_message(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);
        private: CryptoPP::ByteQueue read_message(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData);

        private: bool verify_message_authenticity(CryptoPP::ByteQueue message, const std::string& userid);
        private: bool verify_message_signature(CryptoPP::ByteQueue& message, const std::string& userid);
        private: bool verify_send_time(CryptoPP::ByteQueue& message);
        private: static bool is_authenticated(ServerConnectionData& data);
    };
};

template<typename Key, typename T, typename Base>
CryptoPP::ByteQueue Elonef::ServerConnectionHandler::buid_cache_request_return(
        const std::vector<Key>& keys,
        const ServerConnectionData& connData,
        std::function<std::pair<Base*, T*>(ServerConnectionHandler* _this, const Key& key, const std::string& userid)> fetcher, 
        std::function<CryptoPP::ByteQueue(const T& value)> _toBytes
    ) {
    std::list<std::pair<Key, CryptoPP::ByteQueue>> out;
    for (const Key& key: keys) {
        const std::pair<Base*, T*> value = fetcher(this, key, connData.uid);
        if (value.first == nullptr) {
            continue;
        }
        out.push_back({
            key, _toBytes(*value.second)
        });
        delete value.first;
    }
    return toBytes(out.begin(), out.end());
}
