#include <elonef-communication/handlers/clientConnectionHandler.hpp>
#include <elonef-communication/handlers/return_handlers/promise_return_handler.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/encryption/rsa.hpp>
#include <elonef-communication/encryption/aes.hpp>
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encoding/message.hpp>
#include <elonef-communication/hashing.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

Elonef::ClientConnectionHandler::ClientConnectionHandler(const char* request_uri): 
            MessageHandler(this), 
            data_key_cache(&listToBytes, 0x10),
            signed_key_cache(&listToBytes, 0x14),
            chat_key_cache(&stringQueuePairListToBuffer, 0x16),
            client(ix::customData<ClientConnectionData>())
    {
    this->client.setUrl(
        request_uri
    );

    this->client.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {this->handle_message(this->client, msg);});
}

void Elonef::ClientConnectionHandler::connect() {
    this->start();
}

void Elonef::ClientConnectionHandler::onOpen(ix::WebSocket& conn) {
    
}
void Elonef::ClientConnectionHandler::onClose(ix::WebSocket& conn) {
    
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
    if (this->client.getCustomData<ClientConnectionData>()->auth_data.get() == nullptr) {
        return;
    }

    this->data_key.reset(new CryptoPP::RSA::PrivateKey(Elonef::load_private_rsa(key.data_key)));
    this->sign_key.reset(new Elonef::ECDSA::PrivateKey(Elonef::load_private_ecdsa(key.sign_key)));
    this->user_id.reset(new std::string(key.uid));

    // maybe use a differant waiter, this is only for realy reayl short term holding
    ClientAuthData auth_data = this->client.getCustomData<ClientConnectionData>()->auth_data->get();

    CryptoPP::ByteQueue signature = Elonef::sign_nonstreamable(auth_data.auth_data, *this->sign_key);
    CryptoPP::ByteQueue send_data;
    send_data.Put(0x01);
    Elonef::toBytes(key.uid).TransferAllTo(send_data);
    signature.TransferAllTo(send_data);

    this->send(this->client, auth_data.auth_uuid, send_data, RETURN);

    // delete the auth data
    this->client.getCustomData<ClientConnectionData>()->auth_data.reset();
}

void Elonef::ClientConnectionHandler::start() {
    this->client.start();
}
void Elonef::ClientConnectionHandler::stop() {
    this->client.start();
}


void Elonef::ClientConnectionHandler::wait_for_auth() {
    this->ready.wait();
}

void Elonef::ClientConnectionHandler::load_data_keys(const std::unordered_set<std::string>& keys) {
    this->wait_for_auth();
    this->data_key_cache.ensure_presance<ClientConnectionHandler>(keys, this->client, this, toRsaPublicVector);
}

void Elonef::ClientConnectionHandler::load_signature_keys(const std::unordered_set<std::string>& keys) {
    this->wait_for_auth();
    this->signed_key_cache.ensure_presance<ClientConnectionHandler>(keys, this->client, this, toECDSAPublicVector);
}

void Elonef::ClientConnectionHandler::load_chat_keys(const std::string& chat_key, const CryptoPP::ByteQueue& key_id) {
    this->load_chat_keys({ chat_key, key_id });
}

void Elonef::ClientConnectionHandler::load_chat_keys(const std::pair<std::string, CryptoPP::ByteQueue>& key_id) {
    this->load_chat_keys((std::unordered_set<std::pair<std::string, CryptoPP::ByteQueue>>){key_id});
}

void Elonef::ClientConnectionHandler::load_chat_keys(const std::unordered_set<std::pair<std::string, CryptoPP::ByteQueue>>& key_ids){
    this->wait_for_auth();
    this->chat_key_cache.ensure_presance<ClientConnectionHandler>(key_ids, this->client, this, toChatKeyVector);
}

std::pair<CryptoPP::ByteQueue, CryptoPP::ByteQueue> Elonef::ClientConnectionHandler::get_newest_chat_key(const std::string& str) {
    this->wait_for_auth();
    CryptoPP::ByteQueue data = Elonef::toQueue(str);
    std::shared_ptr<DataWaiter<CryptoPP::ByteQueue>> handler = std::make_shared<DataWaiter<CryptoPP::ByteQueue>>();
    this->send(this->client, data, 0x12, new PromiseReturnHandler(handler));

    CryptoPP::ByteQueue returned_data = handler->get();
    CryptoPP::ByteQueue encrypted_key = extractDynamicLengthQueue(returned_data);
    return {
        decript_rsa(encrypted_key, *this->data_key),
        extractDynamicLengthQueue(returned_data)
    };
}

void Elonef::ClientConnectionHandler::generate_chat_key(const std::unordered_set<std::string>& users, const std::string& chat_id){
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

void Elonef::ClientConnectionHandler::send_message(CryptoPP::ByteQueue& message, const CryptoPP::byte message_type, const std::string& chat_id) {
    std::pair<CryptoPP::ByteQueue, CryptoPP::ByteQueue> newest_key = this->get_newest_chat_key(chat_id);
    
    CryptoPP::ByteQueue message_data = encode_message(message, message_type, chat_id, newest_key.second, newest_key.first, *this->sign_key);
    this->send(this->client, message_data, 0x13);
}

std::vector<Elonef::Message> Elonef::ClientConnectionHandler::read_messages(const std::string& chat_id, const size_t& msg_id, const size_t& amount_of_messages) {
    std::shared_ptr<DataWaiter<CryptoPP::ByteQueue>> handler = std::make_shared<DataWaiter<CryptoPP::ByteQueue>>();
    this->read_messages(chat_id, msg_id, amount_of_messages, new PromiseReturnHandler(handler));
    
    CryptoPP::ByteQueue result = handler->get();
    return this->decode_message(result, chat_id);
}

void Elonef::ClientConnectionHandler::read_messages(const std::string& chat_id, const size_t& msg_id, const size_t& amount_of_messages, ReturnHandler* handler) {
    CryptoPP::ByteQueue to_send = Elonef::toBytes(chat_id);
    Elonef::toBytes(msg_id).TransferAllTo(to_send);
    Elonef::toBytes(amount_of_messages).TransferAllTo(to_send);
    this->send(this->client, to_send, 0x15, handler);
}


CryptoPP::ByteQueue Elonef::ClientConnectionHandler::make_api_request(const std::string& chat_id, CryptoPP::ByteQueue& data, const bool& wait_for_auth) {
    std::shared_ptr<DataWaiter<CryptoPP::ByteQueue>> handler = std::make_shared<DataWaiter<CryptoPP::ByteQueue>>();
    this->make_api_request(chat_id, data, wait_for_auth, new PromiseReturnHandler(handler));
    return handler->get();
}

void Elonef::ClientConnectionHandler::make_api_request(const std::string& call_id, CryptoPP::ByteQueue& data, const bool& wait_for_auth, ReturnHandler* handler) {
    if (wait_for_auth) {
        this->wait_for_auth();
    }
    this->send_api_request(this->client, call_id, data, handler);
}




std::shared_ptr<Elonef::DataWaiter<CryptoPP::RSA::PublicKey>> Elonef::ClientConnectionHandler::get_data_key(const std::string& user) {
    return this->data_key_cache.get(user);
};
std::shared_ptr<Elonef::DataWaiter<Elonef::ECDSA::PublicKey>> Elonef::ClientConnectionHandler::get_signature_key(const std::string& user) {
    return this->signed_key_cache.get(user);
};
std::shared_ptr<Elonef::DataWaiter<CryptoPP::ByteQueue>> Elonef::ClientConnectionHandler::get_chat_key(const std::string& chat, const CryptoPP::ByteQueue& key_id) {
    return this->get_chat_key({chat, key_id});
};
std::shared_ptr<Elonef::DataWaiter<CryptoPP::ByteQueue>> Elonef::ClientConnectionHandler::get_chat_key(const std::pair<std::string, CryptoPP::ByteQueue>& id) {
    return this->chat_key_cache.get(id);
}


std::vector<Elonef::Message> Elonef::ClientConnectionHandler::decode_message(CryptoPP::ByteQueue& queue, const std::string& chat_id) {
    return Elonef::decode_message(queue, *this, chat_id);
}

CryptoPP::ByteQueue Elonef::ClientConnectionHandler::decrypt_chat_key(CryptoPP::ByteQueue& queue) {
    return decript_rsa(queue, *this->data_key);
}

template<typename T>
std::vector<std::pair<std::string, T>> toPublicKeyVecor(CryptoPP::ByteQueue& queue, std::function<T(std::string& key)> load_pub_key) {
    std::vector<std::pair<std::string, CryptoPP::ByteQueue>> keys = Elonef::toIterable
        (queue, Elonef::toStringQueuePair, Elonef::make_vector<std::pair<std::string, CryptoPP::ByteQueue>>);

    std::vector<std::pair<std::string, T>> out(keys.size());
    for (size_t idx=0; idx<keys.size(); idx++) {
        Elonef::SignedKey key = (keys[idx].second);
        out[idx] = {
            keys[idx].first,
            load_pub_key(key.key)
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

bool Elonef::ClientConnectionHandler::is_authenticated(ClientConnectionData& data) {
    return true;
}