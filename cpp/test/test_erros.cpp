#include "run_test.hpp"
#include "test_utils/test.hpp"
#include <elonef-communication/error.hpp>

bool test_security(std::string message) {
    try {
        throw Elonef::SecurityError(message);
    }
    catch (const Elonef::SecurityError& err) {
        return err.what() == message;
    }

    return false;
}

bool test_verification(std::string message) {
    try {
        throw Elonef::VerificationFailed(message);
    }
    catch (const Elonef::SecurityError& err) {
        return err.what() == message;
    }

    return false;
}

void test_error() {
    print_test_header("Errors");
    printSuccess<std::string>("Security", &test_security, "testing shit");
    printSuccess<std::string>("verification", &test_verification, "testing shit");
}