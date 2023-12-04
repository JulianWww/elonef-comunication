#include <elonef-communication/handlers/client_data/client_auth_data.hpp>

Elonef::ClientAuthData::ClientAuthData() {}

Elonef::ClientAuthData::ClientAuthData(CryptoPP::ByteQueue& auth_data, CryptoPP::ByteQueue& auth_uuid ){
    this->auth_data = auth_data;
    this->auth_uuid = auth_uuid;
}