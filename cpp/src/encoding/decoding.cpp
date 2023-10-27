#include <elonef-communication/encoding/decoding.hpp>
#include <elonef-communication/utils.hpp>


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
    CryptoPP::ByteQueue out;
    que.TransferTo(out, size);
    return out;
}