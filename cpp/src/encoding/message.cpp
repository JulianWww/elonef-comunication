#include <elonef-communication/encoding/message.hpp>
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encryption/aes.hpp>
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/utils.hpp>
#include <chrono>
#include <elonef-communication/print.hpp>

CryptoPP::ByteQueue buildSignedMessageBlock(
            const CryptoPP::ByteQueue& message, 
            const CryptoPP::byte& message_type, 
            CryptoPP::ByteQueue key_uuid, 
            CryptoPP::ByteQueue aes_key
        ) {
    CryptoPP::ByteQueue signed_block = Elonef::toBytes(Elonef::get_current_time());
    signed_block.Put(message_type);
    key_uuid.TransferAllTo(signed_block);
    Elonef::encrypt(message, Elonef::toSecBlock(aes_key)).TransferAllTo(signed_block);

    return signed_block;
}

size_t Elonef::get_current_time() {
    return duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

CryptoPP::ByteQueue Elonef::encode_message(
            const CryptoPP::ByteQueue& message,
            const CryptoPP::byte& message_type,
            const std::string& chat_id,
            const CryptoPP::ByteQueue& key_uuid,
            const CryptoPP::ByteQueue& aes_key,
            const Elonef::ECDSA::PrivateKey& signature_key
        ) {
    CryptoPP::ByteQueue message_data = Elonef::toBytes(chat_id);
    CryptoPP::ByteQueue signed_block = buildSignedMessageBlock(message, message_type, key_uuid, aes_key);
    sign(signed_block, signature_key).TransferAllTo(message_data);
    return message_data;
}