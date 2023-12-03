#include <elonef-communication/encryption/aes.hpp>
#include <elonef-communication/utils.hpp>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include "test_utils/test.hpp"
#include "run_test.hpp"

bool test_en_and_decoding(std::string test_msg) {
    CryptoPP::SecByteBlock key = Elonef::randomKey();
    CryptoPP::ByteQueue msg_queue = Elonef::toQueue(test_msg);
    CryptoPP::ByteQueue encrypted = Elonef::encrypt(msg_queue, key);
    CryptoPP::ByteQueue decrypted = Elonef::decrypt(encrypted, key);

    return test_msg == Elonef::toString(decrypted);
}

void test_aes() {
    print_test_header("AES");
    printSuccess<std::string>("en and decrypt", &test_en_and_decoding, "Hello this is a test message that you will never be able to guess");
}