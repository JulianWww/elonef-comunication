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
#include <nlohmann/json.hpp>
#include <iostream>

Elonef::ServerConnectionHandler::ServerConnectionHandler(std::string ip, uint16_t port, const SlidingTimeWindow window,
            std::function<PublicClientKey*(const std::string& id, const std::string& userid)> get_public_key,
            std::function<CryptoPP::ByteQueue*(const std::pair<std::string, CryptoPP::ByteQueue>& chat_key_id, const std::string& userid)> get_chat_key,
            std::function<std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*>(const std::string& chat_id, const std::string& userid)> newest_chat_key_fetcher,
            std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys)> chat_key_setter,
            std::function<void(const std::string& chat_id, const CryptoPP::ByteQueue& message)> message_setter,
            std::function<VectorRange(const std::string& chat_id, const size_t& msg_idx, const size_t& count)> message_fetcher
        ): MessageHandler(this), sendTimeUploadWindow(window), ix::WebSocketServer(ix::customData<ServerConnectionData>(), port, ip) {
    this->get_public_key = get_public_key;
    this->chat_key_fetcher = get_chat_key;
    this->newest_chat_key_fetcher = newest_chat_key_fetcher;
    this->chat_key_setter = chat_key_setter;    
    this->message_setter = message_setter;
    this->message_fetcher = message_fetcher;

    this->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket & webSocket, const ix::WebSocketMessagePtr & msg) {this->handle_message(webSocket, msg);});

    auto res = this->listen();
    if (!res.first)
    {
        throw std::runtime_error(res.second);
    }
}


bool Elonef::ServerConnectionHandler::onWSConnect(
    ix::WebSocket& conn, 
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

void Elonef::ServerConnectionHandler::auth(Elonef::ServerConnectionHandler* _this, ix::WebSocket* conn, CryptoPP::ByteQueue& queue) {
    ServerConnectionData* user_data = conn->getCustomData<ServerConnectionData>();
    user_data->authenticated = false;
    std::string uid = Elonef::toDynamicSizeString(queue);
    Elonef::ECDSA::PublicKey key = Elonef::load_public_ecdsa(_this->get_public_key(uid, uid)->sign_key.key);
    user_data->authenticated = Elonef::verify_nonstreamable(user_data->auth_data, queue, key);
    if (user_data->authenticated) {
        user_data->uid = uid;
        _this->send(*conn, (CryptoPP::byte)0x10);
    }
    else {
        std::cout << "auth kill" << std::endl;
        conn->close();
    }
}

void Elonef::ServerConnectionHandler::onOpen(ix::WebSocket& conn) {
    auto data = Elonef::toQueue(Elonef::randomBytes(1024));
    conn.getCustomData<ServerConnectionData>()->auth_data = data;
    this->send(conn, data, Elonef::AUTH, new Elonef::CallbackReturnHandler<ServerConnectionHandler, ix::WebSocket>(&auth, this, &conn));
}

void Elonef::ServerConnectionHandler::onClose(ix::WebSocket& conn) {

}

void Elonef::ServerConnectionHandler::handle_auth(CryptoPP::ByteQueue& uuid, CryptoPP::ByteQueue& content, ServerConnectionData& connData) {}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::custom_handler(const CryptoPP::byte& type, CryptoPP::ByteQueue& content, ServerConnectionData& connData) {
    if (!connData.authenticated) {
        return CryptoPP::ByteQueue();
    }
    //std::cout << (int) type << std::endl;
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
        case 0x15 : {
            return this->read_message(content, connData);
        }
        case 0x16 : {
            return this->get_chat_key(content, connData);
        }
    }


    return CryptoPP::ByteQueue();
}

void Elonef::ServerConnectionHandler::make_api_request(const std::string& call_id, CryptoPP::ByteQueue& content, const std::unordered_set<std::string>& users, std::function<void(ServerConnectionHandler*, ix::WebSocket*, CryptoPP::ByteQueue&)> callback) {
    for (const std::shared_ptr<ix::WebSocket>& user : this->getClients()) {
        if (users.contains(user->getCustomData<ServerConnectionData>()->uid))
            this->send_api_request(*user.get(), call_id, content, new CallbackReturnHandler(callback, this, user.get()));
    }
}

std::pair<Elonef::PublicClientKey* , Elonef::SignedKey*> Elonef::ServerConnectionHandler::get_signed_key_for_user(ServerConnectionHandler* _this, const std::string& uid, const std::string& requester) {
    PublicClientKey* key = _this->get_public_key(uid, requester);
    if (key == nullptr) {
        return {nullptr, nullptr};
    }
    return std::make_pair(key, &key->sign_key);
}

std::pair<Elonef::PublicClientKey* ,Elonef::SignedKey*> Elonef::ServerConnectionHandler::get_data_key_for_user(ServerConnectionHandler* _this, const std::string& uid, const std::string& requester) {
    PublicClientKey* key = _this->get_public_key(uid, requester);
    if (key == nullptr) {
        return {nullptr, nullptr};
    }
    return {key, &key->data_key};
}

std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*> Elonef::ServerConnectionHandler::get_chat_key_for_user(ServerConnectionHandler* _this, const std::pair<std::string, CryptoPP::ByteQueue>& key_id, const std::string& requester) {
    CryptoPP::ByteQueue* queue = _this->chat_key_fetcher(key_id, requester);
    return {queue, queue};
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_data_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::vector<std::string> keys = toIterable<std::vector<std::string>, std::string>(queue, &toDynamicSizeString, make_vector);
    return this->buid_cache_request_return<std::string, SignedKey, PublicClientKey>(keys, connData, &get_data_key_for_user, &signedKeyToBytes);
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_signature_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::vector<std::string> keys = toIterable<std::vector<std::string>, std::string>(queue, &toDynamicSizeString, make_vector);
    return this->buid_cache_request_return<std::string, SignedKey, PublicClientKey>(keys, connData, &get_signed_key_for_user, &signedKeyToBytes);
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::vector<std::pair<std::string, CryptoPP::ByteQueue>> keys = toIterable<std::vector<std::pair<std::string, CryptoPP::ByteQueue>>, std::pair<std::string, CryptoPP::ByteQueue>>
        (queue, &toStringQueuePair, &make_vector);
    return this->buid_cache_request_return<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue, CryptoPP::ByteQueue>(keys, connData, &get_chat_key_for_user, &toBytes_static_size);
}

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::get_newest_chat_key(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::string chat_id = Elonef::toString(queue);
    std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*> key_and_uuid = this->newest_chat_key_fetcher(chat_id, connData.uid);
    if (key_and_uuid.first == nullptr || key_and_uuid.second == nullptr) {
        return CryptoPP::ByteQueue();
    }
    
    CryptoPP::ByteQueue out = toBytes(key_and_uuid);
    delete key_and_uuid.first;
    delete key_and_uuid.second;
    return out;
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

CryptoPP::ByteQueue Elonef::ServerConnectionHandler::read_message(CryptoPP::ByteQueue& queue, const ServerConnectionData& connData) {
    std::string chat_id = Elonef::toDynamicSizeString(queue);
    size_t first_msg_idx = Elonef::toSize_T(queue);
    size_t msg_count = Elonef::toSize_T(queue);
    VectorRange range = this->message_fetcher(chat_id, first_msg_idx, msg_count);
    return Elonef::toBytes(range.first, range.second);
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
bool Elonef::ServerConnectionHandler::is_authenticated(ServerConnectionData& data) {
    return data.authenticated;
}

