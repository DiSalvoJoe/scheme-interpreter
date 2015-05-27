#include <cxxtest/TestSuite.h>
#include <iostream>

#include "Reader.h"
#include "SchemeTypes.h"
#include "Memory.h"

class TokenizerTest : public CxxTest::TestSuite {
public:
	void testRealisticExample() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        ChunkHeap heap;
        Memory& memory = Memory::getTheMemory();
		Tokenizer tokenizer(memory.getSchemeString("(define (square x)\n\t \t (lambda (x) \n (* x x)))"), heap);

		Token tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_BEGIN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "define");

		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_BEGIN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "square");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "x");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_END);

		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_BEGIN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "lambda");

		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_BEGIN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "x");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_END);

		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_BEGIN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "*");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "x");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "x");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_END);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_END);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_END);

		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
	}

	void testNumbers() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        ChunkHeap heap;
        Memory& memory = Memory::getTheMemory();
		Tokenizer tokenizer(memory.getSchemeString("(+ 124.241 444 -62 -1.32)"), heap);

		Token tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_BEGIN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "+");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_FLOAT(tok, 124.241);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_INT(tok, 444);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_INT(tok, -62);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_FLOAT(tok, -1.32);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, LIST_END);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
	}

	void testEmptyStringAndSpaces() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        ChunkHeap heap;
        Memory& memory = Memory::getTheMemory();

		Tokenizer tokenizerEmpty(memory.getSchemeString(""), heap);
		Token tok = tokenizerEmpty.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizerEmpty.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);

		Tokenizer tokenizerSpaces(memory.getSchemeString("   \n \n     \t    \n    \t"), heap);
		tok = tokenizerSpaces.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizerSpaces.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
	}

	void testNumsAndIdentifiers() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        ChunkHeap heap;
        Memory& memory = Memory::getTheMemory();
		Tokenizer tokenizer(memory.getSchemeString("a4<m!-4->+*?!?!323!5 - -51"), heap);

		Token tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "a4<m!-4->+*?!?!323!5");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_SYMBOL(tok, symbol_table, "-");
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_INT(tok, -51);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
	}

    void testStringLiteral() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Memory& memory = Memory::getTheMemory();
        ChunkHeap heap;
		Tokenizer tokenizer(memory.getSchemeString("\"test \\\" \\n ing\""), heap);
        Token tok = tokenizer.nextToken();
        ASSERT_TOKEN_STRING(tok, "test \" \n ing");
        tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
    }

    void testBoolLiterals() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Memory& memory = Memory::getTheMemory();
        ChunkHeap heap;
		Tokenizer tokenizer(memory.getSchemeString("(if #t xyz #f)"), heap);

        Token tok = tokenizer.nextToken();
        ASSERT_TOKEN_TOKEN(tok, LIST_BEGIN);
        tok = tokenizer.nextToken();
        ASSERT_TOKEN_SYMBOL(tok,symbol_table, "if");
        tok = tokenizer.nextToken();
        ASSERT_TOKEN_BOOL(tok, true);
        tok = tokenizer.nextToken();
        ASSERT_TOKEN_SYMBOL(tok,symbol_table, "xyz");
        tok = tokenizer.nextToken();
        ASSERT_TOKEN_BOOL(tok, false);
        tok = tokenizer.nextToken();
        ASSERT_TOKEN_TOKEN(tok, LIST_END);
        tok = tokenizer.nextToken();
        ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
    }


private:
	void ASSERT_TOKEN_SYMBOL(const Token& tok, SymbolTable& symbol_table, const char sym_name[]) {
		TS_ASSERT(tok.scheme_symbol != nullptr);
		TS_ASSERT_EQUALS(tok.scheme_symbol->type, SYMBOL);
		TS_ASSERT_EQUALS(tok.scheme_symbol->sym,symbol_table.stringToSymbol(sym_name));
		TS_ASSERT_EQUALS(tok.token, IDENTIFIER);
	}
	void ASSERT_TOKEN_TOKEN(const Token& tok, TOKEN tok_type) {
		TS_ASSERT_EQUALS(tok.scheme_symbol, nullptr);
		TS_ASSERT_EQUALS(tok.token, tok_type);
	}
	void ASSERT_TOKEN_INT(const Token& tok, int val) {
		TS_ASSERT(tok.scheme_symbol != nullptr);
		TS_ASSERT_EQUALS(tok.scheme_symbol->type, INT);
		TS_ASSERT_EQUALS(tok.scheme_symbol->integer, val);
		TS_ASSERT_EQUALS(tok.token, INTEGER);
	}
	void ASSERT_TOKEN_FLOAT(const Token& tok, float val) {
		TS_ASSERT(tok.scheme_symbol != nullptr);
		TS_ASSERT_EQUALS(tok.scheme_symbol->type, FLOAT);
		TS_ASSERT_DELTA(tok.scheme_symbol->floatN, val, 0.01);
		TS_ASSERT_EQUALS(tok.token, FLOAT_NUM);
	}
    void ASSERT_TOKEN_STRING(const Token& tok, const char* expected) {
		TS_ASSERT_EQUALS(tok.token, STRING_LITERAL);
		TS_ASSERT(tok.scheme_symbol != nullptr);
		TS_ASSERT_EQUALS(tok.scheme_symbol->type, STRING);
		TS_ASSERT(strcmp(tok.scheme_symbol->string, expected) == 0);
    }
    void ASSERT_TOKEN_BOOL(const Token& tok, bool expected) {
		TS_ASSERT(tok.token == TRUE_LITERAL || tok.token == FALSE_LITERAL);
		TS_ASSERT(tok.scheme_symbol != nullptr);
		TS_ASSERT_EQUALS(tok.scheme_symbol->type, BOOL);
		TS_ASSERT_EQUALS(tok.scheme_symbol->boolean, expected);
    }

};

class ReaderTest : public CxxTest::TestSuite {
public:
    void testReadLiteral() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();

        Object* float_lit = memory.getSchemeString("153.63");
        Object* int_lit = memory.getSchemeString("153");
        Object* bool_lit = memory.getSchemeString("#t");
        Object* sym = memory.getSchemeString("define");

        Reader float_reader(float_lit);
        Reader int_reader(int_lit);
        Reader bool_reader(bool_lit);
        Reader sym_reader(sym);

        Object* floatr = float_reader.read();
        Object* intr = int_reader.read();
        Object* boolr = bool_reader.read();
        Object* symr = sym_reader.read();

        TS_ASSERT_DELTA(floatr->floatN, 153.63, 0.001);
        TS_ASSERT_EQUALS(intr->integer, 153);
        TS_ASSERT_EQUALS(boolr->boolean, true);
        TS_ASSERT_EQUALS(symr->sym, symbol_table.stringToSymbol("define"));

        TS_ASSERT_EQUALS(float_reader.read(), nullptr);
        TS_ASSERT_EQUALS(float_reader.read(), nullptr);
    }

    void testFlatList() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();

        Object* list = memory.getSchemeString("(+ 1 2)");
        Reader reader(list);

        Object* result = reader.read();
        TS_ASSERT_EQUALS(result->type, CONS);
        TS_ASSERT_EQUALS(result->cell.car->type, SYMBOL);
        TS_ASSERT_EQUALS(result->cell.car->sym, symbol_table.stringToSymbol("+"));

        result = result->cell.cdr;
        TS_ASSERT_EQUALS(result->type, CONS);
        TS_ASSERT_EQUALS(result->cell.car->type, INT);
        TS_ASSERT_EQUALS(result->cell.car->integer, 1);

        result = result->cell.cdr;
        TS_ASSERT_EQUALS(result->type, CONS);
        TS_ASSERT_EQUALS(result->cell.car->type, INT);
        TS_ASSERT_EQUALS(result->cell.car->integer, 2);

        result = result->cell.cdr;
        TS_ASSERT_EQUALS(result, nullptr);
    }

    void testEmptyList() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();

        Object* list = memory.getSchemeString("()");
        Reader reader(list);
        TS_ASSERT_EQUALS(reader.read(), nullptr);
    }


};

