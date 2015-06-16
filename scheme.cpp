#include <iostream>
#include <fstream>

#include "Memory.h"
#include "Reader.h"
#include "Evaluator.h"
#include "SchemeTypes.h"

using namespace std;

void runProgram(char* file_name) {
    // read the file into a scheme string
    string line;
    string prog;
    ifstream file;
    file.open(file_name);
    while (getline(file, line)) {
        prog += line;
    }
    ChunkHeap prog_storage;
    Object* program = getSchemeString(prog_storage, prog.c_str());

    // Read, write, and evaluate
    Reader reader(program);
    Evaluator& evaluator = Evaluator::getEvaluator();
    Object* exp;
    try {
        while ((exp = reader.read())) {
            evaluator.eval(exp);
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }
}

void repl() {
    string line;
    ChunkHeap prog_storage;
    Evaluator& evaluator = Evaluator::getEvaluator();
    cout << "scheme: >> ";
    while (getline(cin, line)) {
        Object* exp = getSchemeString(prog_storage, line.c_str());
        Reader reader(exp);
        try {
            write(evaluator.eval(reader.read()), cout);
        } catch (const exception& e) {
            cerr << e.what() << endl;
        }
        cout << endl;
        cout << "scheme: >> ";
    }
    cout << endl;
}

int main(int argc, char** argv) {
    // argv[1] is the name of the program to interpret
    if (argc != 2) {
        repl();
    } else {
        runProgram(argv[1]);
    }
    return 0;
}



