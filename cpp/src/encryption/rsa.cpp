#include <elonef-communication/encryption/rsa.hpp>
#include <elonef-communication/utils.hpp>

CryptoPP::ByteQueue Elonef::encript_rsa(CryptoPP::ByteQueue& toEncrypt, const CryptoPP::RSA::PublicKey& key) {

    CryptoPP::ByteQueue enc;
    CryptoPP::RSAES_OAEP_SHA_Encryptor e(key);
    CryptoPP::PK_EncryptorFilter encryptor(Elonef::rng, e, new CryptoPP::Redirector(enc));
    toEncrypt.TransferTo(encryptor);
    encryptor.MessageEnd();
    return enc;
}

CryptoPP::ByteQueue Elonef::decript_rsa(CryptoPP::ByteQueue& toDecrypt, const CryptoPP::RSA::PrivateKey& key) {
    CryptoPP::ByteQueue dec;
    CryptoPP::RSAES_OAEP_SHA_Decryptor e(key);
    CryptoPP::PK_DecryptorFilter decryptor(Elonef::rng, e, new CryptoPP::Redirector(dec));
    toDecrypt.TransferTo(decryptor);
    decryptor.MessageEnd();

    return dec;
}