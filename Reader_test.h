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
		Tokenizer tokenizer(getSchemeString(memory,"(define (square x)\n\t \t (lambda (x) \n (* x x)))"), heap);

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
		Tokenizer tokenizer(getSchemeString(memory,"(+ 124.241 444 -62 -1.32)"), heap);

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

		Tokenizer tokenizerEmpty(getSchemeString(memory,""), heap);
		Token tok = tokenizerEmpty.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizerEmpty.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);

		Tokenizer tokenizerSpaces(getSchemeString(memory,"   \n \n     \t    \n    \t"), heap);
		tok = tokenizerSpaces.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizerSpaces.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
	}

	void testNumsAndIdentifiers() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        ChunkHeap heap;
        Memory& memory = Memory::getTheMemory();
		Tokenizer tokenizer(getSchemeString(memory,"a4<m!-4->+*?!?!323!5 - -51"), heap);

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
		Tokenizer tokenizer(getSchemeString(memory,"\"test \\\" \\n ing\""), heap);
        Token tok = tokenizer.nextToken();
        ASSERT_TOKEN_STRING(tok, "test \" \n ing");
        tok = tokenizer.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
    }

    void testBoolLiterals() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
        Memory& memory = Memory::getTheMemory();
        ChunkHeap heap;
		Tokenizer tokenizer(getSchemeString(memory,"(if #t xyz #f)"), heap);

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

        Object* float_lit = getSchemeString(memory,"153.63");
        Object* int_lit = getSchemeString(memory,"153");
        Object* bool_lit = getSchemeString(memory,"#t");
        Object* sym = getSchemeString(memory,"define");

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

        Object* list = getSchemeString(memory,"(+ 1 2)");
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

        Object* list = getSchemeString(memory,"()");
        Reader reader(list);
        TS_ASSERT_EQUALS(reader.read(), nullptr);
    }

    void testQuoteAtom() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();

        Object* atom = getSchemeString(memory,"'atom");
        // ['quote' | ] -> ['atom' | X]
        Reader reader(atom);
        Object* obj = reader.read();
        TS_ASSERT_EQUALS(obj->type, CONS);
        TS_ASSERT_EQUALS(obj->cell.car->type, SYMBOL);
        TS_ASSERT_EQUALS(obj->cell.car->sym, symbol_table.stringToSymbol("quote"));
        TS_ASSERT_EQUALS(obj->cell.cdr->type, CONS);
        TS_ASSERT_EQUALS(obj->cell.cdr->cell.car->sym, symbol_table.stringToSymbol("atom"));
        TS_ASSERT_EQUALS(obj->cell.cdr->cell.cdr, nullptr);
    }

    void testQuoteList() {
        Memory& memory = Memory::getTheMemory();
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();

        Object* list = getSchemeString(memory,"'(list 1)");
        // ['quote' | ] -> [ ['list' | ] -> [1 | X] | X]
        Reader reader(list);
        Object* obj = reader.read();
        TS_ASSERT_EQUALS(obj->type, CONS);
        TS_ASSERT_EQUALS(obj->cell.car->type, SYMBOL);
        TS_ASSERT_EQUALS(obj->cell.car->sym, symbol_table.stringToSymbol("quote"));
        TS_ASSERT_EQUALS(obj->cell.cdr->type, CONS);
        TS_ASSERT_EQUALS(obj->cell.cdr->cell.cdr, nullptr);
        Object* quoted_list = obj->cell.cdr->cell.car;

        TS_ASSERT_EQUALS(quoted_list->cell.car->type, SYMBOL);
        TS_ASSERT_EQUALS(quoted_list->cell.car->sym, symbol_table.stringToSymbol("list"));
        TS_ASSERT_EQUALS(quoted_list->cell.cdr->type, CONS);
        TS_ASSERT_EQUALS(quoted_list->cell.cdr->cell.car->type, INT);
        TS_ASSERT_EQUALS(quoted_list->cell.cdr->cell.car->integer, 1);
        TS_ASSERT_EQUALS(quoted_list->cell.cdr->cell.cdr, nullptr);
    }

    void testInfixCons() {
        Memory& memory = Memory::getTheMemory();
        Object* pair = getSchemeString(memory, "(1 . 2");
        Reader reader (pair);
        Object* r = reader.read();
        TS_ASSERT(r);
        TS_ASSERT_EQUALS(r->type, CONS);
        TS_ASSERT_EQUALS(car(r)->integer, 1);
        TS_ASSERT_EQUALS(cdr(r)->integer, 2);
    }

    void testInfixCons2() {
        Memory& memory = Memory::getTheMemory();
        Object* pair = getSchemeString(memory, "(1 2 . 3");
        Reader reader (pair);
        Object* r = reader.read();
        TS_ASSERT(r);
        TS_ASSERT_EQUALS(r->type, CONS);
        TS_ASSERT_EQUALS(car(r)->integer, 1);
        r = cdr(r);

        TS_ASSERT(r);
        TS_ASSERT_EQUALS(r->type, CONS);
        TS_ASSERT_EQUALS(car(r)->integer, 2);
        r = cdr(r);

        TS_ASSERT(r);
        TS_ASSERT_EQUALS(r->type, INT);
        TS_ASSERT_EQUALS(r->integer, 3);
    }





};

