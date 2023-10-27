#include <iostream>
#include "test_utils/test.hpp"
#include "run_test.hpp"



int main(){
    srand(time(0));

    
    test_aes();
    test_ecdsa();
    test_encoding();
    test_rsa();


    return summarize_tests();
}
