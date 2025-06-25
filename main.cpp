#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "ir.h"
using namespace std;

void printMenu() {
    cout << "\nChoose mode:\n";
    cout << "1. Interpret\n";
    cout << "2. Compile to IR and Run\n";
    cout << "3. Both\n";
    cout << "Enter choice (1/2/3): ";
}

int main(int argc, char* argv[]) {
    string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        cout << "Enter the .cpp file to process: ";
        getline(cin, filename);
    }
    ifstream file(filename);
    if (!file) {
        cerr << "Could not open file: " << filename << "\n";
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();

    int choice = 0;
    while (choice < 1 || choice > 3) {
        printMenu();
        string input;
        getline(cin, input);
        if (input == "1" || input == "2" || input == "3") {
            choice = stoi(input);
        } else {
            cout << "Invalid choice. Please enter 1, 2, or 3.\n";
        }
    }

    cout << "\n==============================\n";
    cout << "=== LEXICAL ANALYSIS ===\n";
    auto tokens = tokenize(code);
    for (const auto& token : tokens) {
        cout << "Line " << token.lineNumber << ": "
             << token.value << " [" << tokenTypeToString(token.type) << "]\n";
    }
    cout << "==============================\n";

    cout << "\n=== PARSING & BUILDING AST ===\n";
    Parser parser(code);
    auto tree = parser.parse();
    cout << "\n=== PARSE TREE (ROTATED) ===\n";
    printTree(tree);
    cout << "==============================\n";

    if (choice == 1 || choice == 3) {
        cout << "\n=== INTERPRETER OUTPUT ===\n";
        Interpreter interp;
        try {
            interp.eval(tree);
        } catch (const exception& e) {
            cerr << "Interpreter error: " << e.what() << endl;
        }
        cout << "==============================\n";
    }

    if (choice == 2 || choice == 3) {
        cout << "\n=== COMPILATION TO IR ===\n";
        IRProgram ir;
        int labelCount = 0;
        compileAST(tree, ir, labelCount);
        printIR(ir);
        cout << "==============================\n";
        cout << "\n=== RUNNING IR VM ===\n";
        IRVM vm;
        vm.run(ir);
        cout << "==============================\n";
    }

    return 0;
}
