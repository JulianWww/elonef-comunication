#include "run_test.hpp"
#include "test_utils/test.hpp"
#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/encryption/rsa.hpp>
#include <elonef-communication/utils.hpp>
#include <iostream>

bool test_de_and_encryption(std::string text) {
    auto keys = Elonef::generateKeyPairRSA();

    auto priv_key = Elonef::load_private_rsa(keys.privateKey);
    auto pub_key  = Elonef::load_public_rsa(keys.publicKey);
    
    auto que = Elonef::toQueue(text);
    auto enc = Elonef::encript_rsa(que, pub_key);
    auto dec = Elonef::decript_rsa(enc, priv_key);
    
    std::string text2 = Elonef::toString(dec);

    return text == text2;
}

void test_rsa() {
    print_test_header("RSA");
    printSuccess<std::string>("en and decode", &test_de_and_encryption, "test message");
}