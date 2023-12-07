#pragma once

#include <cryptopp/queue.h>
#include "connection_data.hpp"

namespace Elonef {
    struct ClientAuthData {
        ClientAuthData();
        ClientAuthData(CryptoPP::ByteQueue& auth_data, CryptoPP::ByteQueue& auth_uuid );

        CryptoPP::ByteQueue auth_data;
        CryptoPP::ByteQueue auth_uuid;
    };
}