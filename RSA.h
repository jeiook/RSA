#ifndef RSA_RSA_H
#define RSA_RSA_H

#include <string>

const int MAX_SIZE = 6000;
const int ASCII_MAX_DIGIT = 3;

std::string prependLeadingZeroes(int number, int numDigits);
int modExp(int base, int exponent, int divisor);
int getFirstRelPrime(int number);
int gcd(int a, int b);
int * bezoutCoefficients(int a, int b);
std::string decodeString(const std::string & cipherText);
std::string encodeString(const std::string & plainText);

class RSA {
public:
    RSA();
    ~RSA();

    std::string encrypt(std::string plainText) const;
    std::string decrypt(const std::string & cipherText) const;
    void printKey() const;
    int *getKey() const;
private:
    int getBlockSize() const;
    int *key; // holds the modulus, n, and exponent, e, in an array
    int p;
    int q;
    int d;
    int numCharsInBlock;
    int numDigitsInBlock;
};

#endif //RSA_RSA_H
