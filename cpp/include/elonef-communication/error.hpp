#pragma once

#include <stdexcept>

namespace Elonef {
    class SecurityError : public std::runtime_error {
        public: inline SecurityError(std::string what) : std::runtime_error(what) {}
    };

    class VerificationFailed : public SecurityError {
        public: inline VerificationFailed(std::string what) : SecurityError(what) {}
    };
}