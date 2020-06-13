#include <iostream>
#include <cmath>
#include "RSA.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

string viewEncodedText(string encodedText);

int main() {
    srand(clock());
    RSA rsa;
    rsa.printKey();

    string message;
    string cipherText;
    string plainText;
    string choice;
    string input;

    while (input != "q") {
        cout << "\n\nenter some text to encrypt: " << endl;
        std::getline(cin,message);

        cipherText = rsa.encrypt(message);
        plainText = rsa.decrypt(cipherText);
        cout << "cipherText: " << viewEncodedText(cipherText) << endl;
        cout << "decrypted plainText: " << plainText << endl;

        cout << "Enter 'q' if you would like to stop now, or any other key to continue." << endl;
        std::getline(cin, input);
    }

    return 0;
}

string viewEncodedText(string encodedText) {
    string rawTranslation;
    for (int i = 0; i < encodedText.length(); i+=ASCII_MAX_DIGIT)
        rawTranslation += (char)std::stoi(encodedText.substr(i,ASCII_MAX_DIGIT));

    return  rawTranslation;
}