#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/utils.hpp>
#include <iostream>

CryptoPP::ByteQueue Elonef::toBytes(size_t number) {
    CryptoPP::ByteQueue que;

    while (number > 0) {
        que.Put(number % 256);
        number = number >> 8;
    }
    CryptoPP::ByteQueue out;
    out.Put(que.CurrentSize());
    que.TransferTo(out);

    return out;
}

CryptoPP::ByteQueue Elonef::toBytes_short(size_t number) {
    CryptoPP::ByteQueue que;
    que.Put(number % 256);
    return que;
}
CryptoPP::ByteQueue Elonef::toBytes(std::string str, CryptoPP::ByteQueue(*number_encoder)(size_t num)) {
    auto queue = Elonef::toQueue(str);
    return toBytes(queue, number_encoder);
}

CryptoPP::ByteQueue Elonef::toBytes(CryptoPP::ByteQueue queue, CryptoPP::ByteQueue(*number_encoder)(size_t num)) {
    CryptoPP::ByteQueue out;
    number_encoder(queue.CurrentSize()).TransferAllTo(out);
    queue.TransferAllTo(out);
    return out;    
}