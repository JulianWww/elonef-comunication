#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/utils.hpp>
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encoding/decoding.hpp>
#include <iostream>

CryptoPP::ByteQueue Elonef::sign_nonstreamable(CryptoPP::ByteQueue& data, const Elonef::ECDSA::PrivateKey& key) {
    Elonef::ECDSA::Signer signer (key);
    size_t siglen = signer.MaxSignatureLength();
    CryptoPP::SecByteBlock signature(siglen);
    siglen = signer.SignMessage( rng, (const CryptoPP::byte*)Elonef::toString(data).data(), data.CurrentSize(), (CryptoPP::byte*)&signature[0] );

    CryptoPP::ByteQueue out (siglen);
    out.Put((CryptoPP::byte*)signature.data(), siglen);
    return out;
}

CryptoPP::ByteQueue Elonef::sign(CryptoPP::ByteQueue& data, const Elonef::ECDSA::PrivateKey& key) {
    CryptoPP::ByteQueue data_backup(data);
    CryptoPP::ByteQueue sig = sign_nonstreamable(data_backup, key);
    CryptoPP::ByteQueue outBuff = Elonef::toBytes(sig, Elonef::toBytes_short);
    data.TransferAllTo(outBuff);
    return outBuff;
}

bool Elonef::verify_nonstreamable(CryptoPP::ByteQueue& data, CryptoPP::ByteQueue& signature, const Elonef::ECDSA::PublicKey& key) {
    CryptoPP::SecByteBlock data_block = Elonef::toSecBlock(data);
    CryptoPP::SecByteBlock signature_block = Elonef::toSecBlock(signature);
    Elonef::ECDSA::Verifier verifier (key);
    return verifier.VerifyMessage(data_block.data(), data_block.SizeInBytes(), signature_block.data(), signature_block.SizeInBytes());
}

Elonef::VerificationResult Elonef::verify(CryptoPP::ByteQueue& data, const Elonef::ECDSA::PublicKey& key) {
    CryptoPP::ByteQueue signature = Elonef::extractDynamicLengthQueue(data, &short_toSize_T);
    Elonef::VerificationResult res;

    res.data = CryptoPP::ByteQueue(data);
    res.safe = verify_nonstreamable(data, signature, key);

    return res;
}