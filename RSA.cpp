//
// Created by Jewook Ryu on 6/9/20.
//

#include "RSA.h"
#include <fstream>
#include <iostream>
#include <cmath>

RSA::RSA() {
    key = new int [2];
    int p_index, q_index;
    std::ifstream primes;
    int e = -1;

    while (e == -1) {
        primes.open("../primes.txt"); // file that stores the first ~100,000 primes

        // skip through the file until a value greater than a hundred is reached
        int readNumber;
        int lineNum = 0;
        while (primes >> readNumber) {
            lineNum++;
            if (readNumber >= 357) // approximately the square root of 127127 (needed for convenient block size)
                break;
        }
        // get two indices and skip to these values to obtain p and q
        p_index = rand()%(MAX_SIZE-lineNum-2)+lineNum+1;
        q_index = rand()%(MAX_SIZE-p_index-1)+p_index+1;
        while (primes >> readNumber) {
            lineNum++;
            if (p_index == lineNum) {
                p = readNumber;
                break;
            }
        }
        while (primes >> readNumber) {
            lineNum++;
            if (q_index == lineNum) {
                q = readNumber;
                break;
            }
        }
        primes.close();
        key[0] = p*q;

        e = getFirstRelPrime((p-1)*(q-1));
    }
    key[1] = e;

    // find d (finding d < (p-1)(q-1) is guaranteed as long as e is relatively prime with (p-1)(q-1))
    d = 1;
    while ((d*e)%((p-1)*(q-1)) != 1)
        d++;

    blockSize = getBlockSize(key[0])/ASCII_MAX_DIGIT;
}

RSA::~RSA() {
    delete [] key;
}

void RSA::printKey() const {
    std::cout << "(" << key[0] << ", " << key[1] << ")" << std::endl;
}

std::string RSA::encrypt(std::string plainText) const {
    // divide string into blocks of 3N (since there are 3 digits in the ASCII encoding) digits, where the number
    // 127127...127 with 3N digits does not exceed n.
    int numBlocks = (int)(ceil(plainText.length() / (double)blockSize));

    // pad the message with newlines if necessary
    int extraPadSize = 0;
    while ((plainText.length() + extraPadSize) % blockSize != 0)
        extraPadSize++;
    while (extraPadSize > 0) {
        plainText += "\n";
        extraPadSize--;
    }

    std::string plainTextEncoded [numBlocks];
    for (size_t i = 0; i < numBlocks; i++) {
        for (int j = 0; j < blockSize; j++)
            plainTextEncoded[i] += prependLeadingZeroes(plainText.at(i * blockSize + j), ASCII_MAX_DIGIT);
    }

    // block-wise encryption with modular exponentiation
    std::string rawCipherText;
    for (const std::string & block : plainTextEncoded) {
//        std::cout << prependLeadingZeroes(modExp(std::stoi(block), key[1], key[0]),
//                blockSize*ASCII_MAX_DIGIT) << " "; // debug
        rawCipherText += prependLeadingZeroes(modExp(std::stoi(block), key[1], key[0]),
                blockSize*ASCII_MAX_DIGIT);
    }
    std::cout << std::endl;

    return rawCipherText;
}

std::string RSA::decrypt(const std::string & cipherText) const {
    std::string plainText;
    std::string cipherBlock;
    std::string plainTextBlock;

    // block-wise decryption of cipherText
    for (int blockIndex = 0; blockIndex < cipherText.length()/(blockSize*ASCII_MAX_DIGIT); blockIndex++) {
        cipherBlock = cipherText.substr(blockIndex*blockSize*ASCII_MAX_DIGIT,blockSize*ASCII_MAX_DIGIT);
        plainTextBlock = prependLeadingZeroes(modExp(std::stoi(cipherBlock),d,key[0]),
                blockSize*ASCII_MAX_DIGIT);
        for (int j = 0; j < blockSize; j++)
            plainText += (char)std::stoi(plainTextBlock.substr(j*ASCII_MAX_DIGIT,ASCII_MAX_DIGIT));
    }

    return plainText;
}

int getBlockSize(unsigned long number) {
    int temp = 0;
    std::string numString;
    while (temp <= number) {
        numString += "127"; // max ASCII value
        temp = stoi(numString);
    }

    return (int)numString.length()-ASCII_MAX_DIGIT;
}

std::string prependLeadingZeroes(int number, int numDigits) {
    int currentDigits = (int)floor(log10(number))+1;
    std::string leadingZeroes;
    for (int i = currentDigits; i < numDigits; i++)
        leadingZeroes += "0";
    return leadingZeroes + std::to_string(number);
}

int modExp(int base, int exponent, int divisor) {
    // preliminary work
    int numBinaryDigits = (int)floor(log2(exponent))+1;
    bool expBinary [numBinaryDigits];
    int expTemp = exponent;
    for (int i = 0; i < numBinaryDigits; i++) {
        expBinary[i] = false;
        if (expTemp-pow(2,numBinaryDigits-i-1) >= 0) {
            expBinary[i] = true;
            expTemp -= pow(2,numBinaryDigits-i-1);
        }
    }

    // main algorithm
    long long int x = 1;
    long long int power = base%divisor;

    for (int i = 0; i < numBinaryDigits; i++) {
        if (expBinary[numBinaryDigits-1-i])
            x = (x*power)%divisor;
        power = (power*power)%divisor;
    }

    return x;
}

int getFirstRelPrime(int number) {
    std::ifstream primes;
    primes.open("../primes.txt"); // file that stores the first ~100,000 primes
    int prime;
    while (primes >> prime) {
        if (number%prime != 0) {
            primes.close();
            return prime;
        }
    }

    primes.close();
    return -1;
}