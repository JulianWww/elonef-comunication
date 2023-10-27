#include <elonef-communication/utils.hpp>
#include <crypto++/cryptlib.h>
#include <crypto++/rijndael.h>
#include <crypto++/modes.h>
#include <crypto++/files.h>
#include <crypto++/osrng.h>

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

std::string Elonef::toString(CryptoPP::ByteQueue& bytes) {
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