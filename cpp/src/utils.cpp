#include <elonef-communication/utils.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>

CryptoPP::AutoSeededRandomPool Elonef::rng;

std::string Elonef::toHex(CryptoPP::ByteQueue& bytes)
{
    std::string out;
    CryptoPP::ByteQueue backup(bytes);

    CryptoPP::HexEncoder enc(
        new CryptoPP::StringSink(out)
    );
    backup.TransferAllTo2(enc);
    enc.MessageEnd();

    return out;
}

CryptoPP::ByteQueue Elonef::fromHex(std::string str) {
    CryptoPP::HexDecoder dec;
    dec.Put((unsigned char*)str.data(), str.length());
    CryptoPP::ByteQueue bytes;
    dec.TransferAllTo2(bytes);
    return bytes;
}

std::string Elonef::toString(const CryptoPP::ByteQueue& bytes) {
    std::string out;
    CryptoPP::ByteQueue backup(bytes);

    CryptoPP::StringSink enc(
        out
    );
    backup.TransferAllTo2(enc);
    enc.MessageEnd();

    return out;
}

CryptoPP::SecByteBlock Elonef::toSecBlock(CryptoPP::ByteQueue& bytes) {
    CryptoPP::SecByteBlock out(bytes.CurrentSize());

    CryptoPP::ArraySink enc(
        out.data(), bytes.CurrentSize()
    );
    bytes.TransferAllTo2(enc);
    enc.MessageEnd();

    return out;
}

CryptoPP::ByteQueue Elonef::toQueue(std::string str) { 
    CryptoPP::ByteQueue plain;
    plain.Put(reinterpret_cast<const CryptoPP::byte*>(&str[0]), str.size());
    return plain;
}

CryptoPP::ByteQueue Elonef::toQueue(CryptoPP::SecByteBlock str) {
    CryptoPP::ByteQueue queue;
    queue.Put(str.data(), str.SizeInBytes());
    return queue;
}

CryptoPP::ByteQueue Elonef::get_message(const uint8_t* data, size_t size){
    CryptoPP::ByteQueue queue(size);
    queue.Put((CryptoPP::byte*)data, size);
    return queue;
}