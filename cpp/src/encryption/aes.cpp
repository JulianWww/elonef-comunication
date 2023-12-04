#include <elonef-communication/encryption/aes.hpp>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>



CryptoPP::ByteQueue Elonef::encrypt(CryptoPP::ByteQueue& to_encrypt, const CryptoPP::SecByteBlock& key){
    CryptoPP::SecByteBlock iv = randomBytes(CryptoPP::AES::BLOCKSIZE);
    CryptoPP::ByteQueue cipher;

    cipher.Put(iv.data(), iv.size());

    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc;
    enc.SetKeyWithIV(key, key.size(), iv, iv.size());

    CryptoPP::StreamTransformationFilter f1(enc, new CryptoPP::Redirector(cipher));
    to_encrypt.TransferTo(f1);
    f1.MessageEnd();

    return cipher;    
}

CryptoPP::ByteQueue Elonef::decrypt(CryptoPP::ByteQueue& to_decrypt, const CryptoPP::SecByteBlock& key) {
    CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
    to_decrypt.Get(iv.data(), CryptoPP::AES::BLOCKSIZE);

    CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec;
    dec.SetKeyWithIV(key, key.size(), iv, iv.size());

    CryptoPP::ByteQueue decrypted;

    CryptoPP::StreamTransformationFilter f1(dec, new CryptoPP::Redirector(decrypted));
    to_decrypt.TransferTo(f1);
    f1.MessageEnd();

    return decrypted;
}


CryptoPP::SecByteBlock Elonef::randomBytes(const size_t& size) {
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::SecByteBlock block(size);
    prng.GenerateBlock(block, size);
    return block;
}

CryptoPP::SecByteBlock Elonef::randomKey()
{
    return randomBytes(CryptoPP::AES::DEFAULT_KEYLENGTH);
}