#include <iostream>

void consoleApp(); // function that handles console app (implemented in console.cpp)
void demo(); // function that handles game demo (implemented in demo.cpp)

using std::cout;
using std::endl;
using std::cin;
using std::string;

int main() {
    string input;

    while (input != "q") {
        cout << "Which program would you like to execute?" << endl;
        cout << "(a) Console app" << endl;
        cout << "(b) Demo" << endl;
        getline(cin, input);

        if (input == "a") {
            consoleApp();
            cout << "--------------------------------------------------------------" << endl;
            cout << "This is the end of the console app." << endl;
        }
        else if (input == "b") {
            demo();
            cout << "--------------------------------------------------------------" << endl;
            cout << "This is the end of the demo." << endl;
        }
        else {
            cout << "Sorry, invalid input." << endl << endl;
            continue;
        }

        cout << "Enter 'q' to quit or any other key to go back to the main menu." << endl;
        getline(cin, input);
    }

    return 0;
}
