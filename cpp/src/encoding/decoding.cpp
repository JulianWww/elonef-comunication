#include <elonef-communication/encoding/decoding.hpp>
#include <elonef-communication/utils.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/print.hpp>



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

std::string Elonef::toDynamicSizeString(CryptoPP::ByteQueue& que) {
    return toDynamicSizeString(que, toSize_T);
}

std::string Elonef::toDynamicSizeString(CryptoPP::ByteQueue& que, size_t(*toNum)(CryptoPP::ByteQueue&)) {
    auto str = extractDynamicLengthQueue(que, toNum);
    return Elonef::toString(
        str
    );
}

CryptoPP::ByteQueue Elonef::extractDynamicLengthQueue(CryptoPP::ByteQueue& que) {
    return extractDynamicLengthQueue(que, toSize_T);
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

std::pair<std::string, CryptoPP::ByteQueue> Elonef::toStringQueuePair(CryptoPP::ByteQueue& queue) {
    return toPair(queue, toDynamicSizeString, extractDynamicLengthQueue);
}

std::pair<std::string, std::string> Elonef::toStringStringPair(CryptoPP::ByteQueue& queue) {
    return toPair(queue, toDynamicSizeString, toDynamicSizeString);
}

std::pair<std::pair<std::string, CryptoPP::ByteQueue>, CryptoPP::ByteQueue> Elonef::toChatKeyEntry(CryptoPP::ByteQueue& queue) {
    return toPair(queue, toStringQueuePair, extractDynamicLengthQueue);
}