#pragma once

#include <atomic>
#include "../return_handlers/return_handler.hpp"
#include <mutex>
#include <bst/linked_tree.hpp>

namespace Elonef {
    struct ConnectionData {
        std::atomic_uint64_t active_processes;
        BST::LinkedTree<ReturnHandler> return_uid_map;
        std::mutex uuid_map_mu;
    };
}