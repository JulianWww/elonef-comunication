#include <elonef-communication/print.hpp>
#include <elonef-communication/utils.hpp>

std::ostream &operator<<(std::ostream &ostr, const CryptoPP::ByteQueue val) {
    return (ostr << Elonef::toHex(val));
}

std::ostream &operator<<(std::ostream &ostr, const CryptoPP::SecByteBlock val) {
    return (ostr << Elonef::toQueue(val));
}

std::ostream &operator<<(std::ostream& ostr, const CryptoPP::RSA::PublicKey key) {
    return ostr;
}

std::ostream &operator<<(std::ostream& ostr, const Elonef::ECDSA::PublicKey key) {
    return ostr;
}