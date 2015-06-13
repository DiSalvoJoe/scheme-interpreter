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
        Evaluator& evaluator = Evaluator::getEvaluator();

        Object* obj = getSchemeString(memory,"124");
        Reader reader(obj);

        Object* evalled = evaluator.eval(reader.read(), nullptr);
        TS_ASSERT_EQUALS(evalled->type, INT);
        TS_ASSERT_EQUALS(evalled->integer, 124);
    }

    void testIfWithInts() {
        assertEvalsTo("(if #f 1 2)", "2");
        assertEvalsTo("(if #t 1 2)", "1");
        assertEvalsTo("(if 1 2 3)", "2");
        assertEvalsTo("(if #f 1 2)", "2");
        assertEvalsTo("(if \"hello\" 1 2)", "1");
        assertEvalsTo("(if 125.2 1 2)", "1");
        assertEvalsTo("(if (if #f #f #t) 1 2)", "1");
        assertEvalsTo("(if (if #f #t #f) 1 2)", "2");
        assertEvalsTo("(if 1 (if #f 3 4) 2)", "4");
        assertEvalsTo("(if 1 2 (if #f 3 4))", "2");
    }

    void testDefine() {
        assertEvalsTo("(begin (define testDefine0 (if 1 2 3)) testDefine0)", "2");
        assertEvalsTo("(define testDefine1 1)", "1");
        assertEvalsTo("(begin (define testDefine2 1) testDefine2)", "1");
        assertEvalsTo("(begin (define (always-false) #f) (always-false))", "#f");
        assertEvalsTo("(begin (define (identity f) f) (identity \"hello\"))", "\"hello\"");
    }

    void testBegin() {
        assertEvalsTo("(begin (define testBegin0 (if 1 #f 3)) (if testBegin0 5 6))", "6");
    }

    void testLambda() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Evaluator& evaluator = Evaluator::getEvaluator();

        Object* obj = getSchemeString(memory,"(lambda (x) x) (lambda (x y z) (+ 1 2) 5) (x) (x y z) ((+ 1 2) 5)");
        Reader reader(obj);

        Object* identity_closure = evaluator.eval(reader.read(), nullptr);
        Object* old_body = identity_closure->closure->body;
        Object* add_then_5 = evaluator.eval(reader.read(), nullptr);
        Object* x_list = reader.read();
        Object* xyz_list = reader.read();
        Object* plus12_list = reader.read();

        TS_ASSERT_EQUALS(identity_closure->type, CLOSURE);
        TS_ASSERT_EQUALS(add_then_5->type, CLOSURE);

        TS_ASSERT_EQUALS(identity_closure->closure->env, nullptr);
        TS_ASSERT(equal(identity_closure->closure->parameters, x_list));
        TS_ASSERT(equal(identity_closure->closure->body, x_list));

        TS_ASSERT_EQUALS(add_then_5->closure->env, nullptr);
        TS_ASSERT(equal(add_then_5->closure->parameters, xyz_list));
        TS_ASSERT(equal(add_then_5->closure->body, plus12_list));
    }


    void testSimpleClosures() {
        assertEvalsTo("(begin (define id (lambda (x) x)) (id 5))", "5");
        assertEvalsTo("(begin (define to-bool (lambda (x) (if x #t #f))) (to-bool 7))", "#t");
    }

    void testClosureWithCapturedVariable() {
        assertEvalsTo("(begin (define make-if (lambda (p) (lambda (c) (if p c 0)))) ((make-if #f) 2))", "0");
    }

    void testAddition() {
        assertEvalsTo("(+)", "0");
        assertEvalsTo("(+ 1)", "1");
        assertEvalsTo("(+ -1)", "-1");
        assertEvalsTo("(+ 1.2)", "1.2");
        assertEvalsTo("(+ 1 2 3)", "6");
        assertEvalsTo("(+ 1.1 2 3)", "6.1");
        assertEvalsTo("(+ 1 3 3.5)", "7.5");
        assertEvalsTo("(+ (+ 5 7) (+) (+ 1 4) 1)", "18");
        assertEvalsTo("(+ 1 -2 -6)", "-7");
    }

    void testSubtraction() {
        assertEvalsTo("(-)", "0");
        assertEvalsTo("(- 142)", "142");
        assertEvalsTo("(- -142)", "-142");
        assertEvalsTo("(- 142 1)", "141");
        assertEvalsTo("(- 142 1 2)", "139");
        assertEvalsTo("(- 142 1 -2)", "143");
        assertEvalsTo("(- 142.4)", "142.4");
        assertEvalsTo("(- 142 1.5)", "140.5");
        assertEvalsTo("(- 142.5 2)", "140.5");
        assertEvalsTo("(- (- 3 2 1) (-) (- 8 5 2 1) (- -9 -9))", "0");
    }

    void testMultiplication() {
        assertEvalsTo("(*)", "1");
        assertEvalsTo("(* 2)", "2");
        assertEvalsTo("(* 2.4)", "2.4");
        assertEvalsTo("(* 2.4 1.2)", "2.88");
        assertEvalsTo("(* 2 2.4)", "4.8");
        assertEvalsTo("(* 2 3)", "6");
        assertEvalsTo("(* 2 -3)", "-6");
        assertEvalsTo("(* (* 1) 2 3 (* 4 5))", "120");
    }

    void testDivision() {
        assertEvalsTo("(/)", "1");
        assertEvalsTo("(/ 2)", "2");
        assertEvalsTo("(/ 2 3)", "0");
        assertEvalsTo("(/ 7 3)", "2");
        assertEvalsTo("(/ 3.0 2)", "1.5");
        assertEvalsTo("(/ 3 2.0)", "1.5");
        assertEvalsTo("(/ 3.0 2.0)", "1.5");
        assertEvalsTo("(/ 6 3 2)", "1");
        assertEvalsTo("(/ 6 -3 2)", "-1");
    }

private:
    void assertEvalsTo(const char* lhs, const char* result) {
        Memory& memory = Memory::getTheMemory();
        Evaluator& evaluator = Evaluator::getEvaluator();
        GlobalEnvironment& ge = GlobalEnvironment::getGlobalEnvironment();

        Object* left = getSchemeString(memory,lhs);
        Reader left_reader(left);
        Object* right = getSchemeString(memory,result);
        Reader right_reader(right);

        Object* actual_result = evaluator.eval(left_reader.read(), nullptr);
        if (equal(actual_result, right_reader.read())) {
            TS_ASSERT(true);
        } else {
            std::cout << lhs << "  !=  " << result << std::endl;
            std::cout << lhs << "  =  "; write(actual_result, std::cout); 
            std::cout << std::endl;
            TS_ASSERT(false);
        }
        //memory.clear(); messes with global environment
    }


};
