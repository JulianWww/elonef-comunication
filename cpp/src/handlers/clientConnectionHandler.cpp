#include <elonef-communication/handlers/clientConnectionHandler.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/encoding/encoding.hpp>
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
            data_key_cache(&toRsaPublicVector, &vectorToBytes),
            signed_key_cache(&toECDSAPublicVector, &vectorToBytes)
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

    this->send(this->client, auth_data.auth_uuid, send_data, 0);

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
    //this->wait_for_auth();
    this->data_key_cache.ensure_presance(keys, this->client, this);
    std::cout << this->data_key_cache.print() << std::endl;
}

void Elonef::ClientConnectionHandler::load_signature_keys(const std::vector<std::string>& keys) {
    //this->wait_for_auth();
    this->data_key_cache.ensure_presance(keys, this->client, this);
    std::cout << this->data_key_cache.print() << std::endl;
}