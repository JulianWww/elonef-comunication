#pragma once

#include <cryptopp/queue.h>

namespace Elonef {
    struct ClientAuthData {
        ClientAuthData();
        ClientAuthData(CryptoPP::ByteQueue& auth_data, CryptoPP::ByteQueue& auth_uuid );

        CryptoPP::ByteQueue auth_data;
        CryptoPP::ByteQueue auth_uuid;
    };
}