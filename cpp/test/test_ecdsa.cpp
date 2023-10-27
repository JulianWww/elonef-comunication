#include "run_test.hpp"
#include "test_utils/test.hpp"
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/utils.hpp>


bool test_sig_and_verification(std::string test_msg) {
    auto keypair = Elonef::generateKeyPairECDSA();
    auto priv = Elonef::load_private_ecdsa(keypair.privateKey);
    auto pub  = Elonef::load_public_ecdsa (keypair.publicKey);

    Elonef::ECDSA::Signer signer(priv);

    return false;
}

void test_ecdsa() {
    print_test_header("ECDSA");
    printSuccess<std::string>("sign and verify", &test_sig_and_verification, "Hello this is a test message that you will never be able to guess");
}