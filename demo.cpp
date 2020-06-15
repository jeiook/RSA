#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <ctime>
#include "RSA.h"

using std::cout;
using std::endl;
using std::getline;
using std::cin;
using std::string;
using std::ifstream;

const int MAX_POSITION = 100;

void printField(int hiderPos, int yourPos) {
    int lineLength = MAX_POSITION/10;
    int pos;
    for (int i = 0; i < 20; i++)
        cout << endl;
    for (int i = 0; i < 4*lineLength; i++)
        cout << "_";
    cout << endl;
    for (int i = 0; i < lineLength; i++) {
        for (int j = 1; j <= MAX_POSITION/lineLength; j++) {
            pos = i*10+j;
            cout << std::setw(3);
            if (pos == hiderPos)
                cout << "*";
            else if (pos == yourPos)
                cout << "C";
            else
                cout << " ";
            cout << " ";
        }
        cout << "|" << endl;
    }
    if (hiderPos > 0)
        cout << "hider: *, at " << hiderPos << endl;
    cout << "you: C, at " << yourPos << endl;
}

string bruteForceDecrypt(int * publicKey, const string & cipherText) {
    clock_t start = clock();
    int p = -1;
    int q;
    int d;
    int * bezout;
    ifstream primes;
    primes.open("../primes.txt");
    int prime;
    int count = 0;
    while (primes >> prime && prime <= sqrt(publicKey[0])) {
        count++;
        if (publicKey[0]%prime == 0) {
            p = prime;
            break;
        }
    }
    primes.close();
    if (p == -1)
        return "0";

    q = publicKey[0]/p;
    bezout = bezoutCoefficients(publicKey[1],(p-1)*(q-1));
    d = bezout[0];
    delete [] bezout;

    int numDigitsInBlock;
    int numCharsInBlock;

    {
        long temp = 0;
        std::string numString;
        while (temp <= publicKey[0]) {
            numString += "127"; // max ASCII value
            temp = std::stol(numString);
        }

        numDigitsInBlock = (int)numString.length()-ASCII_MAX_DIGIT;
    }
    numCharsInBlock = numDigitsInBlock/ASCII_MAX_DIGIT;

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
                                                     publicKey[0]),numDigitsInBlock);
        for (int j = 0; j < numCharsInBlock; j++)
            plainText += (char)std::stoi(plainTextBlock.substr(j*ASCII_MAX_DIGIT,ASCII_MAX_DIGIT));
    }

    // remove white space from the right
    while(plainText.at(plainText.length()-1) == '\n')
        plainText = plainText.substr(0,plainText.length()-1);

    clock_t end = clock();
    cout << "this took " << count << " guesses, performed within " << ((double)(end-start)/CLOCKS_PER_SEC) <<
    " seconds." << endl;
    return plainText;
}

void demo() {
    srand(clock());

    string message;
    string cipherText;
    string plainText;
    string input;

    int hiderPosition;
    int yourPosition;

    string cipherPosition;

    RSA * rsa = new RSA();
    rsa->printKey();

    cout << "Background: \n"
                 "You and your friends are playing an improvised version of hide and seek where there is only one hider"
                 ". One of your friends who is now the hider is using an RSA encryptor to hide their position from you."
                 " However, knowing that their encryptor uses small primes to produce the key, which makes it insecure,"
                 " you decide to try to crack the encryption by brute force" << endl << endl;

    yourPosition = 1;
    hiderPosition = rand()%(MAX_POSITION-1) + 2;
    cipherPosition = rsa->encrypt(std::to_string(hiderPosition));
    cout << "done encrypting!" << endl << endl;
    cout << "Friend: Haha, you'll never find me!" << endl;
    cout << "Press any key to continue" << endl;
    getline(cin,input);
    input = "";
    printField(hiderPosition,yourPosition);
    cout << "Press any key to start cryptanalysis" << endl;
    getline(cin,input);
    input = "";
    int foundPosition = std::stoi(bruteForceDecrypt(rsa->getKey(), cipherPosition));
    cout << "Found you at " << foundPosition << "!" << endl;
    cout << "Huh? How did you find me?" << endl;
}
