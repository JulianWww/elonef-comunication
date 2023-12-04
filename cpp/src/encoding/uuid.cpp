#include <elonef-communication/encoding/uuid.hpp>
#include <uuid_v4.h>

UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

CryptoPP::ByteQueue Elonef::uuid() {
    UUIDv4::UUID uuid = uuidGenerator.getUUID();
    CryptoPP::byte bytes[16];
    uuid.bytes((char*)bytes);
    CryptoPP::ByteQueue out;
    out.Put(bytes, 16);
    return out;
}