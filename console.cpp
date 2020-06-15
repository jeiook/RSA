#include <iostream>
#include "RSA.h"

void consoleApp() {
    srand(clock());

    std::string message;
    std::string cipherText;
    std::string plainText;
    std::string input;

    RSA * rsa = new RSA();
    std::cout << "public key: ";
    rsa->printKey();

    while (input.empty()) {
        std::cout << "\n\nenter some text to encrypt: " << std::endl;
        std::getline(std::cin,message);

        std::cout << "public key: ";
        rsa->printKey();
        cipherText = rsa->encrypt(message);
        plainText = rsa->decrypt(cipherText);
        std::cout << "cipher text: " << decodeString(cipherText) << std::endl;
        std::cout << "plaint text: " << plainText << std::endl;

        std::cout << "Press enter if you would like to continue. Press any other key to stop." << std::endl;
        std::getline(std::cin, input);
    }
}