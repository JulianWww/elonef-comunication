#include <iostream>
#include "test_utils/test.hpp"
#include "run_test.hpp"
#include <elonef-communication/handlers/serverConnectionHandler.hpp>
#include <elonef-communication/handlers/clientConnectionHandler.hpp>
#include <elonef-communication/keys/genKeys.hpp>
#include <unistd.h>

std::unordered_map<std::string, std::string> ca_keys;
Elonef::ClientKeys* keys;

Elonef::PublicClientKey* get_public_key(std::string id) {
    return &keys->public_key;
}

int main(){
    keys = new Elonef::ClientKeys(Elonef::generateClientKeys("server,key", ca_keys, "my user id"));
    srand(time(0));


    Elonef::ServerConnectionHandler server("0.0.0.0", 9009, &get_public_key);
    server.run();
    Elonef::ClientConnectionHandler client((unsigned long)3000, "127.0.0.1", 9009, "/", "127.0.0.1:9009");
    client.connect();
    client.run();
    client.authenticate(keys->private_key);
    // sleep(1);
    client.load_data_keys({"test1", "test2b"});
   
//    test_aes();
//    test_ecdsa();
//    test_encoding();
//    test_error();
//    test_rsa();
//
    sleep(1);

    return summarize_tests();
}
