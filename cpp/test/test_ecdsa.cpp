#include "run_test.hpp"
#include "test_utils/test.hpp"
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/utils.hpp>


bool test_sig_and_verification(std::string test_msg) {
    CryptoPP::ByteQueue data = Elonef::toQueue(test_msg);
    Elonef::KeyPair keypair = Elonef::generateKeyPairECDSA();
    Elonef::ECDSA::PrivateKey priv = Elonef::load_private_ecdsa(keypair.privateKey);
    Elonef::ECDSA::PublicKey  pub  = Elonef::load_public_ecdsa (keypair.publicKey);

    CryptoPP::ByteQueue signature = Elonef::sign_nonstreamable(data, priv);
    data = Elonef::toQueue(test_msg);
    bool safe = Elonef::verify_nonstreamable(data, signature, pub);

    return safe;
}

bool test_sig_and_verification_encoding(std::string test_msg) {
    CryptoPP::ByteQueue data = Elonef::toQueue(test_msg);
    Elonef::KeyPair keypair = Elonef::generateKeyPairECDSA();
    Elonef::ECDSA::PrivateKey priv = Elonef::load_private_ecdsa(keypair.privateKey);
    Elonef::ECDSA::PublicKey  pub  = Elonef::load_public_ecdsa (keypair.publicKey);

    CryptoPP::ByteQueue signature = Elonef::sign(data, priv);
    auto res = Elonef::verify(signature, pub);

    return res.safe && Elonef::toString(res.data) == test_msg;
}

void test_ecdsa() {
    print_test_header("ECDSA");
    printSuccess<std::string>("sign and verify nonstream", &test_sig_and_verification, "Hello this is a test message that you will never be able to guess");
    printSuccess<std::string>("sign and verify", &test_sig_and_verification_encoding, "Hello this is a test message that you will never be able to guess");
}