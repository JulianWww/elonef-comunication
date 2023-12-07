#include <elonef-communication/utils.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>

int val =0;

CryptoPP::AutoSeededRandomPool Elonef::rng;

std::string Elonef::toHex(const CryptoPP::ByteQueue& bytes)
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

std::string Elonef::toBase64(CryptoPP::ByteQueue& bytes)
{
    std::string out;
    CryptoPP::ByteQueue backup(bytes);

    CryptoPP::Base64Encoder enc(
        new CryptoPP::StringSink(out),
        false
    );
    backup.TransferAllTo2(enc);
    enc.MessageEnd();

    return out;
}

CryptoPP::ByteQueue Elonef::fromBase64(std::string str) {
    CryptoPP::Base64Decoder dec;
    dec.Put((unsigned char*)str.data(), str.length());
    dec.MessageEnd();
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

CryptoPP::ByteQueue Elonef::toQueue(const CryptoPP::SecByteBlock& str) {
    CryptoPP::ByteQueue queue;
    queue.Put(str.data(), str.SizeInBytes());
    return queue;
}

CryptoPP::ByteQueue Elonef::get_message(const std::string& data){
    CryptoPP::ByteQueue queue(data.size());
    queue.Put((CryptoPP::byte*)data.c_str(), data.size());
    return queue;
}