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
        doTestIntIf("(if #f 1 2)", 2);
        doTestIntIf("(if \"hello\" 1 2)", 1);
        doTestIntIf("(if 125.2 1 2)", 1);
        doTestIntIf("(if (if #f #f #t) 1 2)", 1);
        doTestIntIf("(if (if #f #t #f) 1 2)", 2);
        doTestIntIf("(if 1 (if #f 3 4) 2)", 4);
        doTestIntIf("(if 1 2 (if #f 3 4))", 2);
    }

    void testDefine() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Evaluator evaluator;
        Environment env;

        Object* obj = memory.getSchemeString("(define test (if 1 2 3)) test");
        Reader reader(obj);

        // eval (define test (if 1 2 3))
        Object* evalled = evaluator.eval(reader.read(), &env);
        TS_ASSERT_EQUALS(evalled->type, INT);
        TS_ASSERT_EQUALS(evalled->integer, 2);

        // eval test
        evalled = evaluator.eval(reader.read(), &env);
        TS_ASSERT_EQUALS(evalled->type, INT);
        TS_ASSERT_EQUALS(evalled->integer, 2);
    }

    void testBegin() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Evaluator evaluator;
        Environment env;

        Object* obj = memory.getSchemeString("(begin (define a (if 1 #f 3)) (if a 5 6))");
        Reader reader(obj);

        Object* evalled = evaluator.eval(reader.read(), &env);
        TS_ASSERT_EQUALS(evalled->type, INT);
        TS_ASSERT_EQUALS(evalled->integer, 6);
    }

    void testLambda() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Evaluator evaluator;
        Environment env;

        Object* obj = memory.getSchemeString("(lambda (x) x) (lambda (x y z) (+ 1 2) 5) (x) (x y z) ((+ 1 2) 5)");
        Reader reader(obj);

        Object* identity_closure = evaluator.eval(reader.read(), &env);
        Object* add_then_5 = evaluator.eval(reader.read(), &env);
        Object* x_list = reader.read();
        Object* xyz_list = reader.read();
        Object* plus12_list = reader.read();

        TS_ASSERT_EQUALS(identity_closure->type, CLOSURE);
        TS_ASSERT_EQUALS(add_then_5->type, CLOSURE);

        TS_ASSERT_EQUALS(identity_closure->closure->env, &env);
        TS_ASSERT(equal(identity_closure->closure->parameters, x_list));
        TS_ASSERT(equal(identity_closure->closure->body, x_list));

        TS_ASSERT_EQUALS(add_then_5->closure->env, &env);
        TS_ASSERT(equal(add_then_5->closure->parameters, xyz_list));
        TS_ASSERT(equal(add_then_5->closure->body, plus12_list));
    }


    void testSimpleClosures() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Evaluator evaluator;
        Environment env;
        const char str[] = "(begin (define id (lambda (x) x)) (id 5))";
        const char str2[] = "(begin (define to-bool (lambda (x) (if x #t #f))) (to-bool 7))";
        Object* obj = memory.getSchemeString(str);
        Reader reader(obj);
        Object* evalled = evaluator.eval(reader.read(), &env);
        TS_ASSERT_EQUALS(evalled->type, INT);
        TS_ASSERT_EQUALS(evalled->integer, 5);

        obj = memory.getSchemeString(str2);
        Reader reader2(obj);
        evalled = evaluator.eval(reader2.read(), &env);
        TS_ASSERT_EQUALS(evalled->type, BOOL);
        TS_ASSERT_EQUALS(evalled->boolean, true);
    }

    void testClosureWithCapturedVariable() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Evaluator evaluator;
        Environment env;

        const char str[] = "(begin (define make-if (lambda (p) (lambda (c) (if p c 0)))) ((make-if 1) 2))";
        Object* obj = memory.getSchemeString(str);
        Reader reader(obj);

        Object* evalled = evaluator.eval(reader.read(), &env);
        TS_ASSERT_EQUALS(evalled->type, INT);
        TS_ASSERT_EQUALS(evalled->integer, 2);
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
