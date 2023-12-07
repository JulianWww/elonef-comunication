#pragma once

#include <atomic>

namespace Elonef {
    struct ConnectionData {
        std::atomic_uint64_t active_processes;
    };
}