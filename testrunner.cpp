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
#include "/home/j/prog/scheme-interpreter/Memory_test.h"

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

#include "/home/j/prog/scheme-interpreter/Reader_test.h"

static TokenizerTest suite_TokenizerTest;

static CxxTest::List Tests_TokenizerTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TokenizerTest( "Reader_test.h", 6, "TokenizerTest", suite_TokenizerTest, Tests_TokenizerTest );

static class TestDescription_suite_TokenizerTest_testRealisticExample : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testRealisticExample() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 8, "testRealisticExample" ) {}
 void runTest() { suite_TokenizerTest.testRealisticExample(); }
} testDescription_suite_TokenizerTest_testRealisticExample;

static class TestDescription_suite_TokenizerTest_testNumbers : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testNumbers() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 61, "testNumbers" ) {}
 void runTest() { suite_TokenizerTest.testNumbers(); }
} testDescription_suite_TokenizerTest_testNumbers;

static class TestDescription_suite_TokenizerTest_testEmptyStringAndSpaces : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testEmptyStringAndSpaces() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 87, "testEmptyStringAndSpaces" ) {}
 void runTest() { suite_TokenizerTest.testEmptyStringAndSpaces(); }
} testDescription_suite_TokenizerTest_testEmptyStringAndSpaces;

static class TestDescription_suite_TokenizerTest_testNumsAndIdentifiers : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testNumsAndIdentifiers() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 105, "testNumsAndIdentifiers" ) {}
 void runTest() { suite_TokenizerTest.testNumsAndIdentifiers(); }
} testDescription_suite_TokenizerTest_testNumsAndIdentifiers;

static class TestDescription_suite_TokenizerTest_testStringLiteral : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testStringLiteral() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 123, "testStringLiteral" ) {}
 void runTest() { suite_TokenizerTest.testStringLiteral(); }
} testDescription_suite_TokenizerTest_testStringLiteral;

static class TestDescription_suite_TokenizerTest_testBoolLiterals : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_TokenizerTest_testBoolLiterals() : CxxTest::RealTestDescription( Tests_TokenizerTest, suiteDescription_TokenizerTest, 134, "testBoolLiterals" ) {}
 void runTest() { suite_TokenizerTest.testBoolLiterals(); }
} testDescription_suite_TokenizerTest_testBoolLiterals;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
