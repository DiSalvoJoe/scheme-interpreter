#include <cxxtest/TestSuite.h>
#include "Memory.h"
#include <iostream>

class SymbolTableTest: public CxxTest::TestSuite {
public:
	void testStringToSymbol() {
		char string[50] = "Let's test some symbols!!!";
		char untouched_string[50];
		strcpy(untouched_string, string);
		TS_ASSERT_EQUALS(strcmp(string, untouched_string), 0);
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();

		symbol to5 = symbol_table.stringToSymbol(string,5);
		TS_ASSERT_EQUALS(strcmp(string, untouched_string), 0);

		symbol to5Again = symbol_table.stringToSymbol(string, 5);
		TS_ASSERT_EQUALS(to5, to5Again);

		symbol to15 = symbol_table.stringToSymbol(string, 15);
		TS_ASSERT(to5 != to15);

		symbol to3 = symbol_table.stringToSymbol(string, 3);
		TS_ASSERT(to3 != to15);
		TS_ASSERT(to3 != to5);

		symbol to3Again = symbol_table.stringToSymbol(string, 3);
		TS_ASSERT(to3 == to3Again);
		TS_ASSERT_EQUALS(strcmp(string, untouched_string), 0);

		symbol to3fromUntouched = symbol_table.stringToSymbol(string, 3);
		TS_ASSERT(to3 == to3fromUntouched);
		TS_ASSERT_EQUALS(strcmp(string, untouched_string), 0);
	}

	void testSymbolToString() {
		char string[50] = "more symbol testing!!! @@@";
		char untouched_string[50];
		strcpy(untouched_string, string);
		TS_ASSERT_EQUALS(strcmp(string, untouched_string), 0);
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();

		symbol more = symbol_table.stringToSymbol(string, 4);
		symbol m = symbol_table.stringToSymbol(string, 1);
		symbol mor = symbol_table.stringToSymbol(string, 3);

		TS_ASSERT(strcmp("more", symbol_table.symbolToString(more)) == 0);
		TS_ASSERT(strcmp("mor", symbol_table.symbolToString(mor)) == 0);
		TS_ASSERT(strcmp("m", symbol_table.symbolToString(m)) == 0);
		TS_ASSERT_EQUALS(strcmp(string, untouched_string), 0);
	}

	void testStringToSymbolZeroLengthOrNull() {
		char string[50] = "More symbol testing!!! @@@";
		char untouched_string[50];
		strcpy(untouched_string, string);
		TS_ASSERT_EQUALS(strcmp(string, untouched_string), 0);
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();

		symbol empty = symbol_table.stringToSymbol(string, 0);
		TS_ASSERT(strcmp("", symbol_table.symbolToString(empty)) == 0);
		TS_ASSERT_EQUALS(strcmp(string, untouched_string), 0);

		symbol null = symbol_table.stringToSymbol(nullptr, 36);
		TS_ASSERT(null == nullptr);
		TS_ASSERT_EQUALS(symbol_table.symbolToString(null), nullptr);
	}


};


