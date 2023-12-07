#pragma once

#include "client_auth_data.hpp"
#include "../../data_waiter.hpp"
#include <atomic>

namespace Elonef {
    struct ClientConnectionData : public ConnectionData {
        ClientConnectionData();

        std::atomic_uint64_t active_processes;

        std::unique_ptr<Elonef::DataWaiter<ClientAuthData>> auth_data;
    };
}