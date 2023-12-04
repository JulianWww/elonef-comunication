#pragma once

#include <stdexcept>

#define ELONEF_ERROR(x) class x : public std::runtime_error { public: inline x(std::string what) : std::runtime_error(what) {} }

namespace Elonef {
    ELONEF_ERROR(SecurityError);
    ELONEF_ERROR(VerificationFailed);
    ELONEF_ERROR(ForwardedError);
    ELONEF_ERROR(RemoteError);
}

#undef ELONEF_ERROR