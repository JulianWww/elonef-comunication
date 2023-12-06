#include <elonef-communication/encoding/message.hpp>
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encoding/decoding.hpp>
#include <elonef-communication/encryption/aes.hpp>
#include <elonef-communication/encryption/sign.hpp>
#include <elonef-communication/utils.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <chrono>

#define MSG_LOOP(x) for (Elonef::Message& msg : messages) { if (msg.invalid) {continue;} x}

CryptoPP::ByteQueue buildSignedMessageBlock(
            CryptoPP::ByteQueue& message, 
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
            CryptoPP::ByteQueue& message,
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

Elonef::Message get_base_message(CryptoPP::ByteQueue& queue) {
    Elonef::Message msg;
    Elonef::extractDynamicLengthQueue(queue).TransferAllTo(msg.message);
    return msg;
}


void extract_server_and_user_data(std::vector<Elonef::Message>& messages) {
    MSG_LOOP(
        msg.sender_id = Elonef::toDynamicSizeString(msg.message);
        CryptoPP::ByteQueue signed_block = Elonef::extractDynamicLengthQueue(msg.message);
        msg.upload_time = Elonef::toSize_T(msg.message);

        msg.message.Clear();
        signed_block.TransferAllTo(msg.message);
    );
}

void ensure_ecdsa_key_presance(std::vector<Elonef::Message>& messages, Elonef::ClientConnectionHandler& handler) {
    std::unordered_set<std::string> keys_to_find(messages.size());
    for (
        auto msg_iter = messages.begin(); msg_iter != messages.end(); msg_iter++
    ) {
        keys_to_find.insert(msg_iter->sender_id);
    }
    handler.load_signature_keys(keys_to_find);
}

void check_signatures(std::vector<Elonef::Message>& messages, Elonef::ClientConnectionHandler& handler) {
    MSG_LOOP(
        // check that a fetch is running or has already terminated
        std::shared_ptr<Elonef::DataWaiter<Elonef::ECDSA::PublicKey>> waiter = handler.get_signature_key(msg.sender_id);
        if (waiter == nullptr) {
            msg.invalid = true;
            continue;
        }
        
        // fetch the actuall key
        Elonef::ECDSA::PublicKey key;
        try {
            key = waiter->get();
        }
        catch (...) {
            msg.invalid = true;
            continue;
        }


        //std::cout << Elonef::toHex(msg.message) << std::endl;
        Elonef::VerificationResult res = Elonef::verify(msg.message, key);
        if (!res.safe) {
            msg.invalid = true;
            continue;
        }
        res.data.TransferAllTo(msg.message);
    );
}

void extract_client_data(std::vector<Elonef::Message>& messages) {
    MSG_LOOP(
        msg.send_time = Elonef::toSize_T(msg.message);
        msg.type = Elonef::extreactByte(msg.message);
    );
}

void ensure_aes_key_presance(std::vector<Elonef::Message>& messages, Elonef::ClientConnectionHandler& handler, const std::string& chat_id) {
    std::unordered_set<std::pair<std::string, CryptoPP::ByteQueue>> keys_to_find(messages.size());
    for (
        auto msg_iter = messages.begin(); msg_iter != messages.end(); msg_iter++
    ) {
        CryptoPP::ByteQueue msg_copy = msg_iter->message;
        CryptoPP::ByteQueue key_id = Elonef::extractConstantLengthQueue(msg_copy, ELONEF_UUID_SIZE);
        keys_to_find.insert( { chat_id, key_id } );
    }
    handler.load_chat_keys(keys_to_find);
}

void decrypt_messages(std::vector<Elonef::Message>& messages, Elonef::ClientConnectionHandler& handler, const std::string& chat_id) {
    MSG_LOOP(
        // check that a fetch is running or has already terminated
        CryptoPP::ByteQueue key_id = Elonef::extractConstantLengthQueue(msg.message, ELONEF_UUID_SIZE);
        std::shared_ptr<Elonef::DataWaiter<CryptoPP::ByteQueue>> waiter = handler.get_chat_key(chat_id, key_id);
        if (waiter == nullptr) {
            msg.invalid = true;
            continue;
        }

        CryptoPP::ByteQueue key;
        try {
            key = waiter->get();
        }
        catch (...) {
            msg.invalid = true;
            continue;
        }

        msg.message = Elonef::decrypt(msg.message, Elonef::toSecBlock(key));
    );
}

std::vector<Elonef::Message> Elonef::decode_message(CryptoPP::ByteQueue& data, ClientConnectionHandler& handler, const std::string& chat_id) {
    std::vector<Message> res = toIterable<std::vector<Message>, Message>(data, get_base_message, make_vector);

    extract_server_and_user_data(res);
    ensure_ecdsa_key_presance(res, handler);
    check_signatures(res, handler);
    extract_client_data(res);
    ensure_aes_key_presance(res, handler, chat_id);
    decrypt_messages(res, handler, chat_id);

    return res;
}