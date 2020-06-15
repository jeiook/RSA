//#define BEZOUT_DEBUG // uncomment to see a table of values for each invocation of bezoutCoefficients()

#include "RSA.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>

#ifdef BEZOUT_DEBUG

#include <iomanip>

#endif

RSA::RSA() {
    key = new int [2];
    int p_index, q_index;
    std::ifstream primes;
    std::string testMessage = "1234567890Hello! THIS is A test MESSAGE.";
    std::string testPlainText;
    int n;
    int e;
    int decryptionKey;
    int * bezout;

    while (testMessage != testPlainText.substr(0,testMessage.length())) { // final preliminary test
        decryptionKey = -1;
        numCharsInBlock = -1;
        while (decryptionKey < 1) {
            e = -1;
            while (e == -1) {
                n = 1;
                while (((int)floor(log10(n))+1)%ASCII_MAX_DIGIT != 0) {
                    primes.open("../primes.txt"); // file that stores the first ~10,000 primes

                    // skip through the file until a value greater than ten thousand is reached
                    int readNumber;
                    int lineNum = 0;
                    while (primes >> readNumber) {
                        lineNum++;
                        if (readNumber >= 10000)
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
                    n = p*q;
                }
                key[0] = n;
                e = getFirstRelPrime((p-1)*(q-1));
            }
            key[1] = e;
            // finding a unique d < (p-1)(q-1) is guaranteed by e's relatively primality with (p-1)(q-1)
            bezout = bezoutCoefficients(e,(p-1)*(q-1));
            decryptionKey = bezout[0];
            delete [] bezout;
        }
        d = decryptionKey;
        numDigitsInBlock = getBlockSize();
        numCharsInBlock = numDigitsInBlock / ASCII_MAX_DIGIT;
        testPlainText = decrypt(encrypt(testMessage));
    }
}

RSA::~RSA() {
    delete [] key;
}

void RSA::printKey() const {
    std::cout << "(" << key[0] << ", " << key[1] << ")" << std::endl;
}

std::string RSA::encrypt(std::string plainText) const {
    // pad the message with newlines if necessary
    int extraPadSize = 0;
    while ((plainText.length() + extraPadSize) % numCharsInBlock != 0)
        extraPadSize++;
    while (extraPadSize > 0) {
        plainText += "\n";
        extraPadSize--;
    }

    std::string encodedPlainText = encodeString(plainText);

    // collect the blocks into an array
    int numBlocks = (int)(ceil(plainText.length() / (double)numCharsInBlock));
    std::string encodedBlocks [numBlocks];
    for (int i = 0; i < numBlocks; i++)
        encodedBlocks[i] = encodedPlainText.substr(i*numDigitsInBlock, numDigitsInBlock);

    // block-wise encryption with modular exponentiation
    std::string rawCipherText;
    for (const std::string & block : encodedBlocks) {
        rawCipherText += prependLeadingZeroes(modExp(std::stoi(block), key[1], key[0]),
                                              numDigitsInBlock);
    }

    return rawCipherText;
}

std::string RSA::decrypt(const std::string & cipherText) const {
    int numBlocks = (int)cipherText.length()/numDigitsInBlock;
    std::string encodedCipherBlocks [numBlocks];
    std::string plainText;
    std::string plainTextBlock;

    // separate the cipher text into blocks
    for (int i = 0; i < (int)cipherText.length()/numDigitsInBlock; i++)
        encodedCipherBlocks[i] = cipherText.substr(i * numDigitsInBlock, numDigitsInBlock);

    // block-wise decryption of cipherText
    for (int i = 0; i < (int)cipherText.length()/numDigitsInBlock; i++) {
        plainTextBlock = prependLeadingZeroes(modExp(std::stoi(encodedCipherBlocks[i]),d,
                key[0]),numDigitsInBlock);
        for (int j = 0; j < numCharsInBlock; j++)
            plainText += (char)std::stoi(plainTextBlock.substr(j*ASCII_MAX_DIGIT,ASCII_MAX_DIGIT));
    }

    return plainText;
}

int *RSA::getKey() const {
    int * keyPair = new int [2];
    keyPair[0] = key[0];
    keyPair[1] = key[1];
    return keyPair;
}

int RSA::getBlockSize() const {
    long temp = 0;
    std::string numString;
    while (temp <= key[0]) {
        numString += "127"; // max ASCII value
        temp = std::stol(numString);
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
    // This is a translation of the pseudo-code for fast modular exponentiation found in section 4.2 of the text
    // preliminary work to obtain binary digits of the exponent
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
    primes.open("../primes.txt"); // file that stores the first ~10,000 primes
    int prime;
    while (primes >> prime) {
        if (gcd(number,prime) == 1) {
            primes.close();
            return prime;
        }
    }
    primes.close();

    return -1;
}

int gcd(int a, int b) {
    // A direct translation of a pseudo-code of the Euclidean algorithm in Rosen
    int x = a;
    int y = b;
    int r;
    while (y != 0) {
        r = x%y;
        x = y;
        y = r;
    }
    return x;
}

int * bezoutCoefficients(int a, int b) {
    // an implementation of the extended Euclidean algorithm
    std::vector<int> r_vector;
    std::vector<int> q_vector;
    std::vector<int> s_vector;
    std::vector<int> t_vector;

    r_vector.push_back(a);
    r_vector.push_back(b);
    s_vector.push_back(1);
    s_vector.push_back(0);
    t_vector.push_back(0);
    t_vector.push_back(1);

    int count = 0;
    while (r_vector.back() != 0) {
        q_vector.push_back(r_vector[count]/r_vector[count+1]);
        r_vector.push_back(r_vector[count]%r_vector[count+1]);
        count++;
    }

    for (int j = 2; j <= count; j++) {
        s_vector.push_back(s_vector[j-2]-q_vector[j-2]*s_vector.back());
        t_vector.push_back(t_vector[j-2]-q_vector[j-2]*t_vector.back());
    }

#ifdef BEZOUT_DEBUG
    std::cout << std::setw(2) << "j" << std::setw(10) << "r" << std::setw(10) << "r+1" <<
    std::setw(10) << "q" << std::setw(10) << "r+2" << std::setw(10) << "s" << std::setw(10) << "t" << std::endl;
    for (int j = 0; j < s_vector.size(); j++) {
        std::cout << std::setw(2) << j;
        std::cout << std::setw(10) << r_vector[j];
        std::cout << std::setw(10) << r_vector[j+1];
        std::cout << std::setw(10) << q_vector[j];
        std::cout << std::setw(10) << r_vector[j+2];
        std::cout << std::setw(10) << s_vector[j];
        std::cout << std::setw(10) << t_vector[j];
        std::cout << std::endl;
    }
#endif

    return new int [2] {s_vector.back(), t_vector.back()};
}

std::string decodeString(const std::string & cipherText) {
    std::string rawTranslation;
    for (int i = 0; i < (int)cipherText.length(); i+=ASCII_MAX_DIGIT)
        rawTranslation += (char)std::stoi(cipherText.substr(i, ASCII_MAX_DIGIT));

    return rawTranslation;
}

std::string encodeString(const std::string & plainText) {
    std::string result;
    for (const char & character : plainText)
        result += prependLeadingZeroes((int)character, ASCII_MAX_DIGIT);
    return result;
}