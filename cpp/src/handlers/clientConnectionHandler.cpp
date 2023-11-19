#include <elonef-communication/handlers/clientConnectionHandler.hpp>
#include <elonef-communication/handlers/return_handlers/promise_return_handler.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/encryption/rsa.hpp>
#include <elonef-communication/encryption/aes.hpp>
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encoding/message.hpp>
#include <elonef-communication/hashing.hpp>
#include <elonef-communication/print.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

Elonef::ClientConnectionHandler::ClientConnectionHandler(
        uint64_t timeout, const char* server_ip, uint16_t server_port, const char* request_uri,
        const char* host, const char* origin, const char* protocol,
        const char* extensions, char* resp_protocol, uint32_t resp_protocol_size,
        char* resp_extensions, uint32_t resp_extensions_size): 

            timeout(timeout), server_ip(server_ip), server_port(server_port), request_uri(request_uri),
            host(host), origin(origin), protocol(protocol),
            extensions(extensions), resp_protocol(resp_protocol), resp_protocol_size(resp_extensions_size),
            resp_extensions(resp_extensions), resp_extensions_size(resp_extensions_size),
            MessageHandler(this), 
            data_key_cache(&listToBytes, 0x10),
            signed_key_cache(&listToBytes, 0x14),
            chat_key_cache(&stringQueuePairListToBuffer, 0x16)
    {
}

bool Elonef::ClientConnectionHandler::connect() {
    return this->client.wsConnect(
        this->timeout, this->server_ip, this->server_port, this->request_uri, this->host
    );
}


void Elonef::ClientConnectionHandler::onWSMsg(ELONEF_CLIENT_TYPE::Connection& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len) {
    this->handle_message(conn, payload, pl_len, conn.user_data);
}

void Elonef::ClientConnectionHandler::onWSClose(ELONEF_CLIENT_TYPE::Connection& conn, uint16_t status_code, const char* reason) {
    if (status_code == 1007) {
        std::cout << "auth failed" << std::endl;
    }
    this->stop();
}

void Elonef::ClientConnectionHandler::handle_auth(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ClientConnectionData& connData) {
    connData.auth_data->set_value(ClientAuthData(content, uuid));
}

CryptoPP::ByteQueue Elonef::ClientConnectionHandler::custom_handler(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ClientConnectionData& connData) {
    switch (type) {
        case 0x10: {
            this->ready.set_value(nullptr);
            break;
        }
    }

    return CryptoPP::ByteQueue();
}

void Elonef::ClientConnectionHandler::authenticate(const Elonef::PrivateClientKey& key) {
    if (this->client.user_data.auth_data.get() == nullptr) {
        return;
    }

    this->data_key.reset(new CryptoPP::RSA::PrivateKey(Elonef::load_private_rsa(key.data_key)));
    this->sign_key.reset(new Elonef::ECDSA::PrivateKey(Elonef::load_private_ecdsa(key.sign_key)));
    this->user_id.reset(new std::string(key.uid));

    // maybe use a differant waiter, this is only for realy reayl short term holding
    ClientAuthData auth_data = this->client.user_data.auth_data->get();

    CryptoPP::ByteQueue signature = Elonef::sign_nonstreamable(auth_data.auth_data, *this->sign_key);
    CryptoPP::ByteQueue send_data = Elonef::toBytes(key.uid);
    signature.TransferAllTo(send_data);

    this->send(this->client, auth_data.auth_uuid, send_data, 0x00);

    // delete the auth data
    this->client.user_data.auth_data.reset();
}

void Elonef::ClientConnectionHandler::run_blocking() {
    while (this->running) {
        this->client.poll(this);
        std::this_thread::yield();
        this->clean_executors();
        std::this_thread::yield();
    }
}


void Elonef::ClientConnectionHandler::wait_for_auth() {
    this->ready.wait();
}

void Elonef::ClientConnectionHandler::load_data_keys(const std::vector<std::string>& keys) {
    this->wait_for_auth();
    this->data_key_cache.ensure_presance<ELONEF_CLIENT_TYPE, ClientConnectionHandler>(keys, this->client, this, toRsaPublicVector);
}

void Elonef::ClientConnectionHandler::load_signature_keys(const std::vector<std::string>& keys) {
    this->wait_for_auth();
    this->signed_key_cache.ensure_presance<ELONEF_CLIENT_TYPE, ClientConnectionHandler>(keys, this->client, this, toECDSAPublicVector);
}

void Elonef::ClientConnectionHandler::load_chat_keys(const std::string& chat_key, const CryptoPP::ByteQueue& key_id) {
    this->load_chat_keys({ chat_key, key_id });
}

void Elonef::ClientConnectionHandler::load_chat_keys(const std::pair<std::string, CryptoPP::ByteQueue>& key_id) {
    this->load_chat_keys((std::vector<std::pair<std::string, CryptoPP::ByteQueue>>){key_id});
}

void Elonef::ClientConnectionHandler::load_chat_keys(const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& key_ids){
    this->wait_for_auth();
    this->chat_key_cache.ensure_presance<ELONEF_CLIENT_TYPE, ClientConnectionHandler>(key_ids, this->client, this, toChatKeyVector);
}

std::pair<CryptoPP::ByteQueue, CryptoPP::ByteQueue> Elonef::ClientConnectionHandler::get_newest_chat_key(const std::string& str) {
    this->wait_for_auth();
    CryptoPP::ByteQueue data = Elonef::toQueue(str);
    std::unique_ptr<DataWaiter<CryptoPP::ByteQueue>> handler = std::make_unique<DataWaiter<CryptoPP::ByteQueue>>();
    this->send(this->client, data, 0x12, new PromiseReturnHandler(handler.get()));

    CryptoPP::ByteQueue returned_data = handler->get();
    CryptoPP::ByteQueue encrypted_key = extractDynamicLengthQueue(returned_data);
    return {
        decript_rsa(encrypted_key, *this->data_key),
        extractDynamicLengthQueue(returned_data)
    };
}

void Elonef::ClientConnectionHandler::generate_chat_key(const std::vector<std::string>& users, const std::string& chat_id){
    this->wait_for_auth();
    this->load_data_keys(users);
    CryptoPP::SecByteBlock aes_key = randomKey();

    std::list<std::pair<std::string, CryptoPP::ByteQueue>> keys;
    for (const std::string& user : users) {
        CryptoPP::ByteQueue encrypted_key = toQueue(aes_key);
        keys.push_back({
            user,
            encript_rsa(encrypted_key, this->data_key_cache.get(user)->get())
        });
    }

    CryptoPP::ByteQueue data = toBytes(keys.begin(), keys.end());
    toBytes(chat_id).TransferAllTo(data);
    this->send(this->client, data, 0x11);
}

void Elonef::ClientConnectionHandler::send_message(const CryptoPP::ByteQueue& message, const CryptoPP::byte message_type, const std::string& chat_id) {
    std::pair<CryptoPP::ByteQueue, CryptoPP::ByteQueue> newest_key = this->get_newest_chat_key(chat_id);
    
    CryptoPP::ByteQueue message_data = encode_message(message, message_type, chat_id, newest_key.second, newest_key.first, *this->sign_key);
    this->send(this->client, message_data, 0x13);
}




CryptoPP::ByteQueue Elonef::ClientConnectionHandler::decrypt_chat_key(CryptoPP::ByteQueue& queue) {
    return decript_rsa(queue, *this->data_key);
}

template<typename T>
std::vector<std::pair<std::string, T>> toPublicKeyVecor(CryptoPP::ByteQueue& queue, std::function<T(std::string& key)> load_pub_key) {
    std::vector<std::pair<std::string, std::string>> keys = Elonef::toIterable<std::vector<std::pair<std::string, std::string>>, std::pair<std::string, std::string>>
        (queue, Elonef::toStringStringPair, Elonef::make_vector);


    std::vector<std::pair<std::string, T>> out(keys.size());
    for (size_t idx=0; idx<keys.size(); idx++) {
        nlohmann::json json = nlohmann::json::parse(keys[idx].second);
        std::string key_data = json["key"];
        out[idx] = {
            keys[idx].first,
            load_pub_key(key_data)
        };

    }

    return out;
}

std::vector<std::pair<std::string, CryptoPP::RSA::PublicKey>> Elonef::ClientConnectionHandler::toRsaPublicVector(ClientConnectionHandler* _this, CryptoPP::ByteQueue& queue) {
    return toPublicKeyVecor<CryptoPP::RSA::PublicKey>(queue, load_public_rsa);
}

std::vector<std::pair<std::string, Elonef::ECDSA::PublicKey>> Elonef::ClientConnectionHandler::toECDSAPublicVector(ClientConnectionHandler* _this, CryptoPP::ByteQueue& queue) {
    return toPublicKeyVecor<Elonef::ECDSA::PublicKey>(queue, load_public_ecdsa);
}

std::vector<std::pair<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue>> Elonef::ClientConnectionHandler::toChatKeyVector(ClientConnectionHandler* _this, CryptoPP::ByteQueue& queue) {
    std::vector<std::pair<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue>> keys = 
        toIterable<std::vector<std::pair<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue>>, std::pair<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue>>
        (queue, toChatKeyEntry, make_vector);
    
    for (std::pair<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue>& key : keys) {
        key.second = decript_rsa(key.second, *_this->data_key);
    }
    
    return keys;
}