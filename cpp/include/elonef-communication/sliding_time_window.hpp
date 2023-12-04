#pragma once

#include <cstddef>

namespace Elonef {
    class SlidingTimeWindow {
        private: size_t upper_limit;
        private: size_t lower_limit;

        public: SlidingTimeWindow(size_t upper_limit, size_t lower_limit);
        
        public: bool in_window(size_t value);
    };
}