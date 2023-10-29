#include <iostream>
#include "test_utils/test.hpp"
#include "run_test.hpp"
#include <elonef-communication/handlers/serverConnectionHandler.hpp>

int main(){
    srand(time(0));

    Elonef::ServerConnectionHandler server("0.0.0.0", 9009);
    server.run_blocking();
    
    test_aes();
    test_ecdsa();
    test_encoding();
    test_error();
    test_rsa();

    //server.stop();

    return summarize_tests();
}
