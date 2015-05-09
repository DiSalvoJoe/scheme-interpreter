#include <cctype>
#include "Reader.h"
#include <string>

Tokenizer::Tokenizer(Object* scheme_string):
	symbol_table(SymbolTable::getSymbolTable()),
	memory(Memory::getTheMemory())
{
	index = 0;
	this->scheme_string = scheme_string;
	tokens.push_back(std::pair<std::string, TOKEN>("(", LIST_BEGIN));
	tokens.push_back(std::pair<std::string, TOKEN>(")", LIST_END));
	tokens.push_back(std::pair<std::string, TOKEN>("'", QUOTE));
	tokens.push_back(std::pair<std::string, TOKEN>(".", INFIX_CONS));
	tokens.push_back(std::pair<std::string, TOKEN>("#(", VECTOR_BEGIN));
	tokens.push_back(std::pair<std::string, TOKEN>(")#", VECTOR_END));
	tokens.push_back(std::pair<std::string, TOKEN>("`", BACKQUOTE));
	tokens.push_back(std::pair<std::string, TOKEN>(",", UNQUOTE));
	tokens.push_back(std::pair<std::string, TOKEN>("#t", TRUE_LITERAL));
	tokens.push_back(std::pair<std::string, TOKEN>("#f", FALSE_LITERAL));
	tokens.push_back(std::pair<std::string, TOKEN>("\"", STRING_LITERAL));
}

/*
 * 1) Skip all the whitespace
 * 2) If the string is prefixed by a token, return the corresponding Token and advance the index.
 * 3) Otherwise, the current character is the start of an identifier. Identify the class
 * 		(variable name, float, etc.), and fill a buffer with the identifier.
 */
Token Tokenizer::nextToken() {
	if (!scheme_string) {
		return Token(nullptr, END_TOKEN);
	}

	skipWhiteSpace();
	auto tok_len_pair = getPrefixedToken();
	if (tok_len_pair.first.token == END_TOKEN) {
		// now it can be garbage collected, maybe.
		scheme_string = nullptr;
		return tok_len_pair.first;
	}

	if (tok_len_pair.first.token != NOTHING) {
		index += tok_len_pair.second;
		return tok_len_pair.first;
	}

	return getIdentifierStartingHere();
}

Token Tokenizer::getIdentifierStartingHere() {
	int c = scheme_string->string[index];
	if (isdigit(c)) {
		return getNumber();
	} else if (c == '-' && isdigit(scheme_string->string[index+1])){
		return getNumber();
	} else {
		return getSymbol();
	}
}

Token Tokenizer::getNumber() {
	int c;
	bool is_float = false;
	int negate = scheme_string->string[index] == '-' ? -1 : 1;
	if (negate == -1) {
		++index;
	}
	std::string buffer;
	while ((c = scheme_string->string[index]) && (isdigit(c) || (!is_float && c=='.'))) {
		++index;
		buffer.push_back(c);
		if (c == '.') {
			is_float = true;
		}
	}
	Object* scheme_obj = (Object*)memory.getBytes(sizeof(Object));
	if (is_float) {
		scheme_obj->type = FLOAT;
		scheme_obj->floatN = std::stof(buffer) * negate;
	} else {
		scheme_obj->type = INT;
		scheme_obj->integer = std::stoi(buffer) * negate;
	}
	return Token(scheme_obj, is_float ? FLOAT_NUM : INTEGER);
}

Token Tokenizer::getSymbol() {
	int c;
	std::string buffer;
	while ((c = scheme_string->string[index]) &&
			(isalnum(c) || c == '!' || c == '-' || c == '?' || c == '>' || c == '<'
					|| c == '+' || c == '-' || c == '*' || c == '/')) {
		++index;
		buffer.push_back(c);
	}
	symbol sym = symbol_table.stringToSymbol(buffer.c_str());
	Object* scheme_obj = (Object*)memory.getBytes(sizeof(Object));
	scheme_obj->type = SYMBOL;
	scheme_obj->sym = sym;
	return Token(scheme_obj, IDENTIFIER);
}

void Tokenizer::skipWhiteSpace() {
	int c;
	while ((c = scheme_string->string[index]) && isspace(c)) {
		++index;
	}
}

std::pair<Token, int> Tokenizer::getPrefixedToken() {
	if (! scheme_string->string[index]) {
		return std::pair<Token, int>(Token(nullptr, END_TOKEN), 0);
	}
	TOKEN tok = NOTHING;
	const char* token_prefix;
	int tok_length;
	for (std::pair<std::string, TOKEN> tok_pair : tokens) {
		token_prefix = tok_pair.first.c_str();
		tok_length = strlen(token_prefix);
		if (strncmp(token_prefix, scheme_string->string + index, tok_length) == 0) {
			tok = tok_pair.second;
			return std::pair<Token, int>(Token(nullptr, tok), tok_length);
		}
	}
	return std::pair<Token, int>(Token(nullptr, NOTHING), 0);
}
