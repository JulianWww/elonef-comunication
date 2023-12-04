#include <iostream>
#include "test_utils/test.hpp"
#include "run_test.hpp"
#include <elonef-communication/handlers/serverConnectionHandler.hpp>
#include <elonef-communication/handlers/clientConnectionHandler.hpp>
#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/encryption/aes.hpp>
#include <elonef-communication/encryption/rsa.hpp>
#include <unistd.h>

std::unordered_map<std::string, std::string> ca_keys;
Elonef::ClientKeys* keys;
CryptoPP::ByteQueue chat_key;
CryptoPP::ByteQueue chat_key_id;
std::vector<CryptoPP::ByteQueue> messages;

Elonef::PublicClientKey* get_public_key(const std::string& id, const std::string& userid) {
    return &keys->public_key;
}

CryptoPP::ByteQueue* get_chat_key(const std::pair<std::string, CryptoPP::ByteQueue>& key, const std::string& userid) {
    return &chat_key;
}

std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*> get_newest_chat_key(const std::string& key, const std::string& userid) {
    return {&chat_key, &chat_key_id};
}

void set_chat_key(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys) {
    //std::cout << keys << std::endl;
    //std::cout << chat_key << std::endl;
}

void add_message(const std::string& chat_id, const CryptoPP::ByteQueue& message) {
    messages.push_back(message);
}

std::pair<std::vector<CryptoPP::ByteQueue>::iterator, std::vector<CryptoPP::ByteQueue>::iterator> get_messages(const std::string& chat_id, const size_t& idx, const size_t& size) {
    return {messages.begin(), messages.end()};
}

CryptoPP::ByteQueue test_api(CryptoPP::ByteQueue& queue, Elonef::ServerConnectionData& connData) {
    //throw Elonef::ForwardedError("ups something went wrong");
    return Elonef::toQueue("test success\n\n");
}
CryptoPP::ByteQueue test_api_client(CryptoPP::ByteQueue& queue, Elonef::ClientConnectionData& connData) {
    //throw Elonef::ForwardedError("ups something went wrong");
    return Elonef::toQueue("test success\n\n");
}

int main(){{
    keys = new Elonef::ClientKeys(Elonef::generateClientKeys("server,key", ca_keys, "tester"));
    {
    chat_key_id = Elonef::uuid();
    auto key = Elonef::toQueue(Elonef::randomKey());
    chat_key = Elonef::encript_rsa(key, Elonef::load_public_rsa(keys->public_key.data_key.key));
    }
    srand(time(0));


    Elonef::ServerConnectionHandler server("0.0.0.0", 9008, Elonef::SlidingTimeWindow(10, 30), &get_public_key, &get_chat_key, &get_newest_chat_key, &set_chat_key, &add_message, &get_messages);
    server.add_api_callback("test", test_api, true);
    server.start();
    Elonef::ClientConnectionHandler client("ws://127.0.0.1:9008");
    client.add_api_callback("test", test_api_client);
    client.connect();
    client.authenticate(keys->private_key);
    sleep(1);
    //client.load_data_keys({"test1", "test2b"});
    //client.load_chat_keys("test1", Elonef::uuid());
    //std::cout << client.get_newest_chat_key("test Chat") << std::endl;
    //client.generate_chat_key({"test_user", "eris"}, "test_chat");
    CryptoPP::ByteQueue message = Elonef::toQueue("test message");
    //client.send_message(message, 0x00, "test_chat");
    //sleep(1);
    //std::cout << client.read_messages("test_chat", 0, 10) << std::endl;
    std::cout << Elonef::toString(client.make_api_request("test", message, true));
    
    server.make_api_request("tester", message, {"tester"}, [](Elonef::ServerConnectionHandler* handler, ix::WebSocket* sock, CryptoPP::ByteQueue& data){});
    test_aes();
    test_ecdsa();
    test_encoding();
    test_error();
    test_rsa();
    sleep(1);
}
    return summarize_tests();
}
