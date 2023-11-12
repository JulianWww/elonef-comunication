#include <elonef-communication/encoding/decoding.hpp>
#include <elonef-communication/utils.hpp>
#include <nlohmann/json.hpp>
#include <elonef-communication/keys/load_keys.hpp>



size_t Elonef::toSize_T(CryptoPP::ByteQueue& que)
{
    short length = short_toSize_T(que);
    size_t out=0;
    for (short idx=0; idx<length; idx++) {
        short next = short_toSize_T(que);
        out = out + (next << (idx*8));
    }
    return out;
}

size_t Elonef::short_toSize_T(CryptoPP::ByteQueue& que)
{
    CryptoPP::byte val;
    que.Get(val);
    return int(val);
}

std::string Elonef::toDynamicSizeString(CryptoPP::ByteQueue& que, size_t(*toNum)(CryptoPP::ByteQueue&)) {
    auto str = extractDynamicLengthQueue(que, toNum);
    return Elonef::toString(
        str
    );
}

CryptoPP::ByteQueue Elonef::extractDynamicLengthQueue(CryptoPP::ByteQueue& que, size_t(*toNum)(CryptoPP::ByteQueue&)) {
    size_t size = toNum(que);
    return extractConstantLengthQueue(que, size);
}

CryptoPP::ByteQueue Elonef::extractConstantLengthQueue(CryptoPP::ByteQueue& que, size_t size) {
    CryptoPP::ByteQueue out(size);
    que.TransferTo(out, size);
    return out;
}

CryptoPP::byte Elonef::extreactByte(CryptoPP::ByteQueue& queue) {
    CryptoPP::byte byte;
    queue.Get(byte);
    return byte;
}

std::vector<std::string> Elonef::toStringVector(CryptoPP::ByteQueue& queue) {
    return toIterable<std::vector<std::string>, std::string>(queue, &toDynamicSizeString_long, &make_vector);
}

std::vector<std::pair<std::string, CryptoPP::RSA::PublicKey>> Elonef::toRsaPublicVector(CryptoPP::ByteQueue& queue) {
    std::vector<std::string> keys = toStringVector(queue);
    std::vector<std::pair<std::string, CryptoPP::RSA::PublicKey>> out;

    for (std::string& key : keys) {
        nlohmann::json key_json = nlohmann::json::parse(key);

        if (!key_json["user_id"].is_string() || !key_json["key"].is_string()) {
            continue;
        }

        out.push_back({key_json["user_id"], load_public_rsa(key_json["key"])});
    }
    return out;
}

std::vector<std::pair<std::string, Elonef::ECDSA::PublicKey>> Elonef::toECDSAPublicVector(CryptoPP::ByteQueue& queue) {
std::vector<std::string> keys = toStringVector(queue);
    std::vector<std::pair<std::string, Elonef::ECDSA::PublicKey>> out;

    for (std::string& key : keys) {
        nlohmann::json key_json = nlohmann::json::parse(key);

        if (!key_json["user_id"].is_string() || !key_json["key"].is_string()) {
            continue;
        }

        out.push_back({key_json["user_id"], load_public_ecdsa(key_json["key"])});
    }
    return out;
}