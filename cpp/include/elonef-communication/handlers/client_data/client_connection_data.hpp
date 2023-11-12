#pragma once

#include "client_auth_data.hpp"
#include "../../data_waiter.hpp"

namespace Elonef {
    struct ClientConnectionData {
        ClientConnectionData();

        std::unique_ptr<Elonef::DataWaiter<ClientAuthData>> auth_data;
    };
}