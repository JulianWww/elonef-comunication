#include <elonef-communication/encoding/decoding.hpp>
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encryption/aes.hpp>
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/handlers/serverConnectionHandler.hpp>
#include <elonef-communication/handlers/return_handlers/callback_return_handler.hpp>
#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/utils.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

Elonef::ServerConnectionHandler::ServerConnectionHandler(std::string ip, uint16_t port,
            std::function<PublicClientKey*(std::string id)> get_public_key
        ): MessageHandler(this) {
    this->get_public_key = get_public_key;

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

void auth(Elonef::ServerConnectionHandler* _this, ELONEF_SERVER_TYPE::Connection* conn, CryptoPP::ByteQueue& queue) {
    conn->user_data.authenticated = false;
    std::string uid = Elonef::toDynamicSizeString(queue);
    Elonef::ECDSA::PublicKey key = Elonef::load_public_ecdsa(_this->get_public_key(uid)->sign_key.key);
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
    switch (type) {
        case 0x10 : {
            return this->get_data_key(content);
        };
        case 0x14 : {
            return this->get_signature_key(content);
        }
    }


    return CryptoPP::ByteQueue();
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_public_key_encoder(CryptoPP::ByteQueue& queue, std::function<SignedKey*(PublicClientKey* key)> key_getter) {
    std::vector<std::string> users = Elonef::toStringVector(queue);
    std::vector<std::string> keys;

    for (std::string user : users) {
        PublicClientKey* key = this->get_public_key(user);
        if (key == nullptr) {
            continue;
        }
        SignedKey* sig = key_getter(key);
        nlohmann::json out;
        out["key"] = sig->key;
        out["signatures"] = sig->signatures;
        out["user_id"] = user;
        keys.push_back(out.dump());
    }

    return Elonef::vectorToBytes(keys);
}

Elonef::SignedKey* get_signature_key_from_pub(Elonef::PublicClientKey* key) {
    return &key->sign_key;
}

Elonef::SignedKey* get_data_key_from_pub(Elonef::PublicClientKey* key) {
    return &key->data_key;
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_data_key(CryptoPP::ByteQueue& queue) {
    return this->get_public_key_encoder(queue, get_data_key_from_pub);
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_signature_key(CryptoPP::ByteQueue& queue) {
    return this->get_public_key_encoder(queue, get_signature_key_from_pub);
}

void Elonef::ServerConnectionHandler::run_blocking() {
    while (this->running) {
        this->server.poll(this);
        std::this_thread::yield();
        this->clean_executors();
        std::this_thread::yield();
    }
}