#ifndef READER_H
#define READER_H

#include "SchemeTypes.h"
#include "Memory.h"

#include <string>
#include <vector>

enum TOKEN {
	LIST_BEGIN,
	LIST_END,
	INFIX_CONS,

	IDENTIFIER,
	FLOAT_NUM,
	INTEGER,

	VECTOR_BEGIN,
	VECTOR_END,

	BACKQUOTE,
	UNQUOTE,
	QUOTE,

	TRUE_LITERAL,
	FALSE_LITERAL,
	STRING_LITERAL,

	END_TOKEN,

	NOTHING
};

class Token {
public:
	Token(Object* sym, TOKEN tok) : scheme_symbol(sym), token(tok) {}
	Object* scheme_symbol;
	TOKEN token;
};

class Tokenizer {
public:
	Tokenizer(Object* scheme_string, ChunkHeap& heap);
	Token nextToken();

private:
	// Advance index until scheme_string->string[index] is no longer white space.
	void skipWhiteSpace();

	// Return the token that prefixes the string and the length of the token's corresponding string
	// If no token exists, it returns {NOTHING, 0}
	std::pair<Token,int> getPrefixedToken();

	// Return a Token with token type = IDENTIFIER, FLOAT_NUM, or INTEGER and a corresponding scheme_symbol
	Token getIdentifierStartingHere();

	Token getNumber();
	Token getSymbol();
    Token getStringLiteral();

	SymbolTable& symbol_table;
	ChunkHeap& memory;
	Object* scheme_string;
	int index;
	std::vector<std::pair<std::string, TOKEN>> tokens;
};

class Reader {
public:
    Reader(Object* obj, ChunkHeap& heap);
	Object* read();
private:
    //Object* dispatchRead();
    //Object* readList();

    Tokenizer token_stream;
    ChunkHeap& memory;
};

#endif
