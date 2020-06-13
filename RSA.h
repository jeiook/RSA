//
// Created by Jewook Ryu on 6/9/20.
//

#ifndef RSA_RSA_H
#define RSA_RSA_H

#include <string>

const int MAX_SIZE = 160;
const int ASCII_MAX_DIGIT = 3;

int getBlockSize(unsigned long number);
std::string prependLeadingZeroes(int number, int numDigits);
int modExp(int base, int exponent, int divisor); // Translation of Rosen's algorithm 5 in section 4.2 for fast modular
// exponentiation
int getFirstRelPrime(int number);

class RSA {
public:
    RSA();
    ~RSA();

    std::string encrypt(std::string plainText) const;
    std::string decrypt(const std::string & cipherText) const;
    void printKey() const;
private:
    int * key;
    int p;
    int q;
    int d;
    int blockSize;
};

#endif //RSA_RSA_H
