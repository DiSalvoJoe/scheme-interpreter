#include <cctype>
#include <string>
#include <assert.h>

#include "Reader.h"
#include "SchemeTypes.h"


Tokenizer::Tokenizer(Object* scheme_string, ChunkHeap& heap) :
	symbol_table(SymbolTable::getSymbolTable()),
	memory(heap)
{
	index = 0;
	this->scheme_string = scheme_string;
	tokens.push_back(std::pair<std::string, TOKEN>("(", LIST_BEGIN));
	tokens.push_back(std::pair<std::string, TOKEN>(")", LIST_END));
	tokens.push_back(std::pair<std::string, TOKEN>("'", QUOTE));
	tokens.push_back(std::pair<std::string, TOKEN>(".", INFIX_CONS));
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
    switch (tok_len_pair.first.token) {
        case END_TOKEN:
            // now it can be garbage collected, maybe.
            scheme_string = nullptr;
            return tok_len_pair.first;
        case NOTHING:
            return getIdentifierStartingHere();
        case STRING_LITERAL:
            return getStringLiteral();
        case TRUE_LITERAL:
        case FALSE_LITERAL:
            index += tok_len_pair.second; // advance past token
            tok_len_pair.first.scheme_symbol = getObject(memory, BOOL);
            tok_len_pair.first.scheme_symbol->boolean = (tok_len_pair.first.token == TRUE_LITERAL);
            return tok_len_pair.first;
        default:
            index += tok_len_pair.second;
            return tok_len_pair.first;
    }
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
	Object* scheme_obj = getObject(memory, INT);
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
    int end_index = index;
	while ((c = scheme_string->string[end_index]) &&
			(isalnum(c) || c == '!' || c == '-' || c == '?' || c == '>' || c == '<'
					|| c == '+' || c == '-' || c == '*' || c == '/' || c == '=')) {
		++end_index;
	}
	symbol sym = symbol_table.stringToSymbol(scheme_string->string + index, end_index - index);
	Object* scheme_obj = getObject(memory, SYMBOL);
	scheme_obj->sym = sym;
    index = end_index;
	return Token(scheme_obj, IDENTIFIER);
}

Token Tokenizer::getStringLiteral() {
    // precondition: index marks beginning ", so advance index
    ++index;
    int c;
    std::string buffer;
    while ((c = scheme_string->string[index]) && c != '"') {
        if (c == '\\') {
            c = scheme_string->string[++index];
            if (c == 'n') {
                c = '\n';
            }
            // Otherwise, we'll assume for now that c is " or \, which
            // should be pushed onto the buffer as is. Eventually,
            // throw an exception if c is an invalid escape character.
        }
        buffer.push_back(c);
        ++index;
    }
    ++index; // skip ending "
    return Token(getSchemeString(memory, buffer.c_str()), STRING_LITERAL);
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

Reader::Reader(Object* obj) :
    token_stream(Tokenizer(obj, memory))
     {}



Object* Reader::read() {
    Memory& mem = Memory::getTheMemory();
    Object* result = dispatchRead().first;
    mem.requireBytes(size(result));
    return copy(result);
}

std::pair<Object*,bool> Reader::dispatchRead() {
    Token tok = token_stream.nextToken();
    Object* result = nullptr;
    bool infix_cons = false;
    switch(tok.token) {
        case LIST_BEGIN:
            result = readList();
            break;
        case QUOTE:
            result = readQuote();
            break;
        case UNQUOTE:
            result = readUnquote();
            break;
        case BACKQUOTE:
            result = readBackQuote();
            break;
        case LIST_END:
            break;
        case END_TOKEN:
            break;
        case NOTHING:
            break;
        case INFIX_CONS:
            infix_cons = true;
            result = dispatchRead().first;
            break;
        default:
            result = tok.scheme_symbol;
            break;
    }
    return std::pair<Object*,bool>(result, infix_cons);
}

Object* Reader::readList() {
    Object* result = nullptr;
    Object* element = nullptr;
    std::pair<Object*,bool> p;
    p = dispatchRead();
    while (p.first && !p.second) {
        element = p.first;
        Object* link = getObject(memory, CONS);
        link->cell.cdr = result;
        link->cell.car = element;
        result = link;
        p = dispatchRead();
    } 
    if (p.first && p.second) {
        Object* tail = result;
        result = reverseList(result);
        cdr(tail) = p.first;
        p = dispatchRead();
        assert(!p.first);
    } else {
        result = reverseList(result);
    }
    return result;
}

Object* Reader::readBackQuote() {
    SymbolTable& symbol_table = SymbolTable::getSymbolTable();
    symbol unquote_sym = symbol_table.stringToSymbol("unquote");
    Object* result = dispatchRead().first;
    if (result && result->type != CONS) {
        result = package(result, "quote");
    } else if (result && result->type == CONS) {
        Object* cur = result;
        while (cur && cur->type == CONS) {
            if (! (car(cur)->type == CONS && caar(cur)->type == SYMBOL
                        && caar(cur)->sym == unquote_sym)) {
                car(cur) = package(car(cur), "quote");
            }
            cur = cdr(cur);
        }
        result = package(result, "backquote");
    }
    return result;
}

Object* Reader::readQuote() {
    return package(dispatchRead().first, "quote");
}

Object* Reader::readUnquote() {
    return package(dispatchRead().first, "unquote");
}

Object* Reader::package(Object* obj, const char* sym) {
    SymbolTable& symbol_table = SymbolTable::getSymbolTable();
    Object* package_head = getObject(memory, CONS);
    Object* package_tail = getObject(memory, CONS);
    package_head->cell.car = getObject(memory, SYMBOL);
    package_head->cell.car->sym = symbol_table.stringToSymbol(sym);
    package_head->cell.cdr = package_tail;
    package_tail->cell.car = obj;
    package_tail->cell.cdr = nullptr;
    return package_head;
}
