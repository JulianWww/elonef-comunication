#include "run_test.hpp"
#include "test_utils/test.hpp"
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encoding/decoding.hpp>
#include <elonef-communication/utils.hpp>
#include <elonef-communication/encoding/uuid.hpp>
#include <elonef-communication/encryption/aes.hpp>


#define LIST { rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand() }

bool test_uuid(size_t counts) {
    auto uuid = Elonef::uuid();
    std::vector<std::string> uuids(counts);
    for (auto iter=uuids.begin(); iter!=uuids.end(); iter++) {
        auto uuid = Elonef::uuid();
        *iter = Elonef::toHex(uuid);
    }
    std::sort(uuids.begin(), uuids.end());
    int uniqueCount = std::unique(uuids.begin(), uuids.end()) - uuids.begin();

    return uniqueCount == counts;
}

bool test_size_t(size_t value) {
    auto enc = Elonef::toBytes(value);
    enc.Put(48);
    size_t out = Elonef::toSize_T(enc);
    return value == out;
}

bool test_short(size_t value) {
    auto enc = Elonef::toBytes(value);
    enc.Put(48);
    size_t out = Elonef::toSize_T(enc);
    return value == out;
}

bool test_string(std::string txt) {
    auto enc = Elonef::toBytes(txt);
    enc.Put(48);
    auto out = Elonef::toDynamicSizeString(enc);
    
    return txt == out;
}

bool test_array(std::vector<int> arr) {
    auto enc = Elonef::toBytes(arr.begin(), arr.end());
    enc.Put(48);
    auto dec = Elonef::toIterable<std::vector<size_t>>(enc, &Elonef::toSize_T, &Elonef::make_vector);
    return iterableEqual(arr.begin(), arr.end(), dec.begin(), dec.end());
}

bool test_list(std::list<int> arr) {
    auto enc = Elonef::toBytes(arr.begin(), arr.end());
    enc.Put(48);
    auto dec = Elonef::toIterable<std::list<size_t>>(enc, &Elonef::toSize_T, &Elonef::make_list);
    return iterableEqual(arr.begin(), arr.end(), dec.begin(), dec.end());
}

bool test_hex(CryptoPP::SecByteBlock que) {
    CryptoPP::ByteQueue source;
    source.Put(que.data(), que.SizeInBytes());
    auto hex = Elonef::toHex(source);
    auto out = Elonef::fromHex(hex);
    return Elonef::toHex(source) == Elonef::toHex(source);
}

void test_encoding() {
    print_test_header("Encoding");

    printSuccess<size_t>("size_t", &test_size_t, rand());
    printSuccess<size_t>("short", &test_size_t, rand()%256);
    printSuccess<std::string>("string", &test_string, "hello");
    printSuccess<std::vector<int>>("Vector", &test_array, LIST);
    printSuccess<std::vector<int>>("list", &test_array, LIST);
    printSuccess<size_t>("uuid", &test_uuid, 4096);
    printSuccess<CryptoPP::SecByteBlock>("hex", &test_hex, Elonef::randomBytes(1024));
}