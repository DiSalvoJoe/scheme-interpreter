#include <cxxtest/TestSuite.h>
#include "Reader.h"
#include "Memory.h"

class TokenizerTest : public CxxTest::TestSuite {
public:
	void testRealisticExample() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
		Tokenizer tokenizer(makeSchemeStr("(define (square x)\n\t \t (lambda (x) \n (* x x)))"));

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
		Tokenizer tokenizer(makeSchemeStr("(+ 124.241 444 -62 -1.32)"));

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
		Tokenizer tokenizerEmpty(makeSchemeStr(""));
		Token tok = tokenizerEmpty.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizerEmpty.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);

		Tokenizer tokenizerSpaces(makeSchemeStr("   \n \n     \t    \n    \t"));
		tok = tokenizerSpaces.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
		tok = tokenizerSpaces.nextToken();
		ASSERT_TOKEN_TOKEN(tok, END_TOKEN);
	}

	void testNumsAndIdentifiers() {
		SymbolTable& symbol_table = SymbolTable::getSymbolTable();
		Tokenizer tokenizer(makeSchemeStr("a4<m!-4->+*?!?!323!5 - -51"));

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
	Object* makeSchemeStr(const char str[]) {
		Memory& memory = Memory::getTheMemory();
		int len = strlen(str)+1;
		// must allocate string and object together, otherwise garbage collection could
		// happen in between, causing one to point to a no-longer valid memory location.
		char* new_str = memory.getBytes(len+sizeof(Object));
		Object* scheme_str = (Object*)(new_str+len);
		strcpy(new_str, str);
		scheme_str->type = STRING;
		scheme_str->string = new_str;
		return scheme_str;
	}


};


