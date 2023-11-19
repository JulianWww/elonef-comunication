#include <elonef-communication/encoding/decoding.hpp>
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encoding/message.hpp>
#include <elonef-communication/encryption/aes.hpp>
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/handlers/serverConnectionHandler.hpp>
#include <elonef-communication/handlers/return_handlers/callback_return_handler.hpp>
#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/utils.hpp>
#include <elonef-communication/print.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

Elonef::ServerConnectionHandler::ServerConnectionHandler(std::string ip, uint16_t port, const SlidingTimeWindow window,
            std::function<const PublicClientKey*(const std::string& id, const std::string& userid)> get_public_key,
            std::function<const CryptoPP::ByteQueue*(const std::pair<std::string, CryptoPP::ByteQueue>& chat_key_id, const std::string& userid)> get_chat_key,
            std::function<const std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*>(const std::string& chat_id, const std::string& userid)> newest_chat_key_fetcher,
            std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys)> chat_key_setter,
            std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& message)> message_setter
        ): MessageHandler(this), sendTimeUploadWindow(window) {
    this->get_public_key = get_public_key;
    this->chat_key_fetcher = get_chat_key;
    this->newest_chat_key_fetcher = newest_chat_key_fetcher;
    this->chat_key_setter = chat_key_setter;    
    this->message_setter = message_setter;

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
    return true;
}

void Elonef::ServerConnectionHandler::auth(Elonef::ServerConnectionHandler* _this, ELONEF_SERVER_TYPE::Connection* conn, CryptoPP::ByteQueue& queue) {
    conn->user_data.authenticated = false;
    std::string uid = Elonef::toDynamicSizeString(queue);
    Elonef::ECDSA::PublicKey key = Elonef::load_public_ecdsa(_this->get_public_key(uid, uid)->sign_key.key);
    conn->user_data.authenticated = Elonef::verify_nonstreamable(conn->user_data.auth_data, queue, key);
    if (conn->user_data.authenticated) {
        conn->user_data.uid = uid;
        _this->send(*conn, (CryptoPP::byte)0x10);
    }
    else {
        conn->close();
    }
}

void Elonef::ServerConnectionHandler::onWSConnectionEstablished(ELONEF_SERVER_TYPE::Connection& conn) {
    auto data = Elonef::toQueue(Elonef::randomBytes(1024));
    conn.user_data.auth_data = data;
    this->send(conn, data, Elonef::AUTH, new Elonef::CallbackReturnHandler<ServerConnectionHandler, ELONEF_SERVER_TYPE::Connection>(&auth, this, &conn));
}

void Elonef::ServerConnectionHandler::onWSMsg(ELONEF_SERVER_TYPE::Connection& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len){
    this->handle_message(conn, payload, pl_len, conn.user_data);
}

void Elonef::ServerConnectionHandler::onWSClose(ELONEF_SERVER_TYPE::Connection& conn, uint16_t status_code, const char* reason){
    std::cout << "connection closed" << std::endl;
}

void Elonef::ServerConnectionHandler::handle_auth(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ServerConnectionData& connData) {}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::custom_handler(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ServerConnectionData& connData) {
    if (!connData.authenticated) {
        return CryptoPP::ByteQueue();
    }
    std::cout << (int) type << std::endl;
    switch (type) {
        case 0x10 : {
            return this->get_data_key(content, connData);
        };
        case 0x11 : {
            this->set_chat_key(content, connData);
            break;
        }
        case 0x12 : {
            return this->get_newest_chat_key(content, connData);
        }   
        case 0x13 : {
            this->add_message(content, connData);
            break;
        }
        case 0x14 : {
            return this->get_signature_key(content, connData);
        }
        case 0x16 : {
            return this->get_chat_key(content, connData);
        }
    }


    return CryptoPP::ByteQueue();
}

const Elonef::SignedKey* Elonef::ServerConnectionHandler::get_signed_key_for_user(ServerConnectionHandler* _this, const std::string& uid, const std::string& requester) {
    const PublicClientKey* key = _this->get_public_key(uid, requester);
    if (key == nullptr) {
        return nullptr;
    }
    return &key->sign_key;
}

const Elonef::SignedKey* Elonef::ServerConnectionHandler::get_data_key_for_user(ServerConnectionHandler* _this, const std::string& uid, const std::string& requester) {
    const PublicClientKey* key = _this->get_public_key(uid, requester);
    if (key == nullptr) {
        return nullptr;
    }
    return &key->data_key;
}

const CryptoPP::ByteQueue* Elonef::ServerConnectionHandler::get_chat_key_for_user(ServerConnectionHandler* _this, const std::pair<std::string, CryptoPP::ByteQueue>& key_id, const std::string& requester) {
    return _this->chat_key_fetcher(key_id, requester);
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_data_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::vector<std::string> keys = toIterable<std::vector<std::string>, std::string>(queue, &toDynamicSizeString, make_vector);
    return this->buid_cache_request_return<std::string, SignedKey>(keys, connData, &get_data_key_for_user, &signedKeyToBytes);
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_signature_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::vector<std::string> keys = toIterable<std::vector<std::string>, std::string>(queue, &toDynamicSizeString, make_vector);
    return this->buid_cache_request_return<std::string, SignedKey>(keys, connData, &get_data_key_for_user, &signedKeyToBytes);
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::vector<std::pair<std::string, CryptoPP::ByteQueue>> keys = toIterable<std::vector<std::pair<std::string, CryptoPP::ByteQueue>>, std::pair<std::string, CryptoPP::ByteQueue>>
        (queue, &toStringQueuePair, &make_vector);
    return this->buid_cache_request_return<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue>(keys, connData, &get_chat_key_for_user, &toBytes_static_size);
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_newest_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::string chat_id = Elonef::toString(queue);
    std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*> key_and_uuid = this->newest_chat_key_fetcher(chat_id, connData.uid);
    if (key_and_uuid.first == nullptr || key_and_uuid.second == nullptr) {
        return CryptoPP::ByteQueue();
    }
    
    return toBytes(key_and_uuid);
}

void Elonef::ServerConnectionHandler::set_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::vector<std::pair<std::string, CryptoPP::ByteQueue>> keys = toIterable<std::vector<std::pair<std::string, CryptoPP::ByteQueue>>,std::pair<std::string, CryptoPP::ByteQueue>> 
        (queue, toStringQueuePair, make_vector);
    std::string chat_id = toDynamicSizeString(queue);
    auto _uuid = uuid();
    this->chat_key_setter(chat_id, _uuid, keys);
}

void Elonef::ServerConnectionHandler::add_message(CryptoPP::ByteQueue& client_message, const ServerConnectionData& connData) {
    std::string chat_id = toDynamicSizeString(client_message);
    if (!this->verify_message_authenticity(client_message, connData.uid)) {
        return;
    }
    CryptoPP::ByteQueue message = Elonef::toBytes(connData.uid);
    toBytes(client_message).TransferAllTo(message);
    toBytes(get_current_time()).TransferAllTo(message);
    message.Put(0x00);

    this->message_setter(connData.uid, message);

}


bool Elonef::ServerConnectionHandler::verify_message_authenticity(CryptoPP::ByteQueue message, const std::string& userid) {
    return this->verify_message_signature(message, userid);
}

bool Elonef::ServerConnectionHandler::verify_message_signature(CryptoPP::ByteQueue& message, const std::string& userid) {
    const PublicClientKey* keyptr = this->get_public_key(userid, userid);
    if (keyptr == nullptr) {
        return false;
    }
    Elonef::ECDSA::PublicKey key = load_public_ecdsa(keyptr->sign_key.key);
    VerificationResult verified = verify(message, key);
    return verified.safe && this->verify_send_time(verified.data);
}

bool Elonef::ServerConnectionHandler::verify_send_time(CryptoPP::ByteQueue& message) {
    size_t send_time = toSize_T(message);
    return this->sendTimeUploadWindow.in_window(send_time);
}



void Elonef::ServerConnectionHandler::run_blocking() {
    while (this->running) {
        this->server.poll(this);
        std::this_thread::yield();
        this->clean_executors();
        std::this_thread::yield();
    }
}