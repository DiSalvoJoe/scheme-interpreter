/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "cxxtest";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_SymbolTableTest_init = false;
#include "/home/j/prog/scheme_interpreter/scheme interpreter/Memory_test.h"

static SymbolTableTest suite_SymbolTableTest;

static CxxTest::List Tests_SymbolTableTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SymbolTableTest( "Memory_test.h", 5, "SymbolTableTest", suite_SymbolTableTest, Tests_SymbolTableTest );

static class TestDescription_suite_SymbolTableTest_testStringToSymbol : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_SymbolTableTest_testStringToSymbol() : CxxTest::RealTestDescription( Tests_SymbolTableTest, suiteDescription_SymbolTableTest, 7, "testStringToSymbol" ) {}
 void runTest() { suite_SymbolTableTest.testStringToSymbol(); }
} testDescription_suite_SymbolTableTest_testStringToSymbol;

static class TestDescription_suite_SymbolTableTest_testSymbolToString : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_SymbolTableTest_testSymbolToString() : CxxTest::RealTestDescription( Tests_SymbolTableTest, suiteDescription_SymbolTableTest, 36, "testSymbolToString" ) {}
 void runTest() { suite_SymbolTableTest.testSymbolToString(); }
} testDescription_suite_SymbolTableTest_testSymbolToString;

static class TestDescription_suite_SymbolTableTest_testStringToSymbolZeroLengthOrNull : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_SymbolTableTest_testStringToSymbolZeroLengthOrNull() : CxxTest::RealTestDescription( Tests_SymbolTableTest, suiteDescription_SymbolTableTest, 53, "testStringToSymbolZeroLengthOrNull" ) {}
 void runTest() { suite_SymbolTableTest.testStringToSymbolZeroLengthOrNull(); }
} testDescription_suite_SymbolTableTest_testStringToSymbolZeroLengthOrNull;

#include "/home/j/prog/scheme_interpreter/scheme interpreter/Reader_test.h"

static TokenizerTest suite_TokenizerTest;

static CxxTest::List Tests_TokenizerTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TokenizerTest( "Reader_test.h", 8, "TokenizerTest", suite_TokenizerTest, Tests_TokenizerTest );

static class TestDescription_suite_TokenizerTest_testRealisticExample : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testRealisticExample() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 10, "testRealisticExample" ) {}
 void runTest() { suite_TokenizerTest.testRealisticExample(); }
} testDescription_suite_TokenizerTest_testRealisticExample;

static class TestDescription_suite_TokenizerTest_testNumbers : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testNumbers() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 63, "testNumbers" ) {}
 void runTest() { suite_TokenizerTest.testNumbers(); }
} testDescription_suite_TokenizerTest_testNumbers;

static class TestDescription_suite_TokenizerTest_testEmptyStringAndSpaces : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testEmptyStringAndSpaces() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 89, "testEmptyStringAndSpaces" ) {}
 void runTest() { suite_TokenizerTest.testEmptyStringAndSpaces(); }
} testDescription_suite_TokenizerTest_testEmptyStringAndSpaces;

static class TestDescription_suite_TokenizerTest_testNumsAndIdentifiers : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testNumsAndIdentifiers() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 107, "testNumsAndIdentifiers" ) {}
 void runTest() { suite_TokenizerTest.testNumsAndIdentifiers(); }
} testDescription_suite_TokenizerTest_testNumsAndIdentifiers;

static class TestDescription_suite_TokenizerTest_testStringLiteral : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testStringLiteral() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 125, "testStringLiteral" ) {}
 void runTest() { suite_TokenizerTest.testStringLiteral(); }
} testDescription_suite_TokenizerTest_testStringLiteral;

static class TestDescription_suite_TokenizerTest_testBoolLiterals : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testBoolLiterals() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 136, "testBoolLiterals" ) {}
 void runTest() { suite_TokenizerTest.testBoolLiterals(); }
} testDescription_suite_TokenizerTest_testBoolLiterals;

static ReaderTest suite_ReaderTest;

static CxxTest::List Tests_ReaderTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ReaderTest( "Reader_test.h", 197, "ReaderTest", suite_ReaderTest, Tests_ReaderTest );

static class TestDescription_suite_ReaderTest_testReadLiteral : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ReaderTest_testReadLiteral() : CxxTest::RealTestDescription( Tests_ReaderTest, suiteDescription_ReaderTest, 199, "testReadLiteral" ) {}
 void runTest() { suite_ReaderTest.testReadLiteral(); }
} testDescription_suite_ReaderTest_testReadLiteral;

static class TestDescription_suite_ReaderTest_testFlatList : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ReaderTest_testFlatList() : CxxTest::RealTestDescription( Tests_ReaderTest, suiteDescription_ReaderTest, 227, "testFlatList" ) {}
 void runTest() { suite_ReaderTest.testFlatList(); }
} testDescription_suite_ReaderTest_testFlatList;

static class TestDescription_suite_ReaderTest_testEmptyList : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ReaderTest_testEmptyList() : CxxTest::RealTestDescription( Tests_ReaderTest, suiteDescription_ReaderTest, 253, "testEmptyList" ) {}
 void runTest() { suite_ReaderTest.testEmptyList(); }
} testDescription_suite_ReaderTest_testEmptyList;

static class TestDescription_suite_ReaderTest_testQuoteAtom : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ReaderTest_testQuoteAtom() : CxxTest::RealTestDescription( Tests_ReaderTest, suiteDescription_ReaderTest, 262, "testQuoteAtom" ) {}
 void runTest() { suite_ReaderTest.testQuoteAtom(); }
} testDescription_suite_ReaderTest_testQuoteAtom;

static class TestDescription_suite_ReaderTest_testQuoteList : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ReaderTest_testQuoteList() : CxxTest::RealTestDescription( Tests_ReaderTest, suiteDescription_ReaderTest, 278, "testQuoteList" ) {}
 void runTest() { suite_ReaderTest.testQuoteList(); }
} testDescription_suite_ReaderTest_testQuoteList;

#include "/home/j/prog/scheme_interpreter/scheme interpreter/Evaluator_test.h"

static EvaluatorTest suite_EvaluatorTest;

static CxxTest::List Tests_EvaluatorTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_EvaluatorTest( "Evaluator_test.h", 9, "EvaluatorTest", suite_EvaluatorTest, Tests_EvaluatorTest );

static class TestDescription_suite_EvaluatorTest_testEvalInt : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testEvalInt() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 11, "testEvalInt" ) {}
 void runTest() { suite_EvaluatorTest.testEvalInt(); }
} testDescription_suite_EvaluatorTest_testEvalInt;

static class TestDescription_suite_EvaluatorTest_testIfWithInts : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testIfWithInts() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 24, "testIfWithInts" ) {}
 void runTest() { suite_EvaluatorTest.testIfWithInts(); }
} testDescription_suite_EvaluatorTest_testIfWithInts;

static class TestDescription_suite_EvaluatorTest_testDefine : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testDefine() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 37, "testDefine" ) {}
 void runTest() { suite_EvaluatorTest.testDefine(); }
} testDescription_suite_EvaluatorTest_testDefine;

static class TestDescription_suite_EvaluatorTest_testBegin : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testBegin() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 45, "testBegin" ) {}
 void runTest() { suite_EvaluatorTest.testBegin(); }
} testDescription_suite_EvaluatorTest_testBegin;

static class TestDescription_suite_EvaluatorTest_testLambda : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testLambda() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 49, "testLambda" ) {}
 void runTest() { suite_EvaluatorTest.testLambda(); }
} testDescription_suite_EvaluatorTest_testLambda;

static class TestDescription_suite_EvaluatorTest_testSimpleClosures : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testSimpleClosures() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 77, "testSimpleClosures" ) {}
 void runTest() { suite_EvaluatorTest.testSimpleClosures(); }
} testDescription_suite_EvaluatorTest_testSimpleClosures;

static class TestDescription_suite_EvaluatorTest_testClosureWithCapturedVariable : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testClosureWithCapturedVariable() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 82, "testClosureWithCapturedVariable" ) {}
 void runTest() { suite_EvaluatorTest.testClosureWithCapturedVariable(); }
} testDescription_suite_EvaluatorTest_testClosureWithCapturedVariable;

static class TestDescription_suite_EvaluatorTest_testAddition : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testAddition() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 86, "testAddition" ) {}
 void runTest() { suite_EvaluatorTest.testAddition(); }
} testDescription_suite_EvaluatorTest_testAddition;

static class TestDescription_suite_EvaluatorTest_testSubtraction : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testSubtraction() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 98, "testSubtraction" ) {}
 void runTest() { suite_EvaluatorTest.testSubtraction(); }
} testDescription_suite_EvaluatorTest_testSubtraction;

static class TestDescription_suite_EvaluatorTest_testMultiplication : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testMultiplication() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 111, "testMultiplication" ) {}
 void runTest() { suite_EvaluatorTest.testMultiplication(); }
} testDescription_suite_EvaluatorTest_testMultiplication;

static class TestDescription_suite_EvaluatorTest_testDivision : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_EvaluatorTest_testDivision() : CxxTest::RealTestDescription( Tests_EvaluatorTest, suiteDescription_EvaluatorTest, 122, "testDivision" ) {}
 void runTest() { suite_EvaluatorTest.testDivision(); }
} testDescription_suite_EvaluatorTest_testDivision;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
