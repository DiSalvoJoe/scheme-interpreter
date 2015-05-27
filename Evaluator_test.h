#include <cxxtest/TestSuite.h>
#include <iostream>

#include "Reader.h"
#include "SchemeTypes.h"
#include "Memory.h"
#include "Evaluator.h"

class EvaluatorTest : public CxxTest::TestSuite {
public:
    void testEvalInt() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Evaluator evaluator;

        Object* obj = memory.getSchemeString("124");
        Reader reader(obj);

        Object* evalled = evaluator.eval(reader.read(), nullptr);
        TS_ASSERT_EQUALS(evalled->type, INT);
        TS_ASSERT_EQUALS(evalled->integer, 124);
    }

    void testIfWithInts() {
        doTestIntIf("(if #f 1 2)", 2);
        doTestIntIf("(if #t 1 2)", 1);
        doTestIntIf("(if 1 2 3)", 2);
        doTestIntIf("(if '() 1 2)", 2);
        doTestIntIf("(if \"hello\" 1 2)", 1);
        doTestIntIf("(if 125.2 1 2)", 1);
        doTestIntIf("(if (if #f #f #t) 1 2)", 1);
        doTestIntIf("(if (if #f #t #f) 1 2)", 2);
        doTestIntIf("(if 1 (if #f 3 4) 2)", 4);
        doTestIntIf("(if 1 2 (if #f 3 4))", 2);
    }


private:
    void doTestIntIf(const char* if_str, int result) {
        Memory& memory = Memory::getTheMemory();
        Evaluator evaluator;

        Object* obj = memory.getSchemeString(if_str);
        Reader reader(obj);
        Object* evalled = evaluator.eval(reader.read(), nullptr);
        TS_ASSERT_EQUALS(evalled->type, INT);
        TS_ASSERT_EQUALS(evalled->integer, result);
    }


};
