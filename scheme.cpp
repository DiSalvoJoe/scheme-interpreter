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
    //cout << prog << endl;
    ChunkHeap prog_storage;
    Object* program = getSchemeString(prog_storage, prog.c_str());

    // Read, write, and evaluate
    Reader reader(program);
    Evaluator& evaluator = Evaluator::getEvaluator();
    Object* exp;
    while ((exp = reader.read())) {
        evaluator.eval(exp);
    }
}

void repl() {
    cout << "REPL will be implemented 'soon.'" << endl;
}

int main(int argc, char** argv) {
    // argv[1] is the name of the program
    if (argc != 2) {
        repl();
    } else {
        runProgram(argv[1]);
    }
    return 0;
}



