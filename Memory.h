#ifndef MEMORY_H
#define MEMORY_H

#include <map>
#include <cstring>

typedef char* symbol;
class Reader;
class Evaluator;

class SymbolTable {
public:

	 static SymbolTable& getSymbolTable();
	~SymbolTable();

	symbol stringToSymbol(const char* string_start, int str_len);
	symbol stringToSymbol(const char* string);
	char* symbolToString(symbol) const;

	//Object* symbolToSchemeString(symbol);

private:

	SymbolTable();
	SymbolTable(SymbolTable const&) = delete;
	void operator=(SymbolTable const&) = delete;

	struct cmp_str  {
		bool operator()(char const *a, char const *b) {
			return std::strcmp(a, b) < 0;
		}
	};

	std::map<char*, symbol, cmp_str> table;

};

class Memory {
public:
	static Memory& getTheMemory();
	~Memory();

	char* getBytes(size_t bytes);

private:
	Memory(const int initial_heap_size);

	Memory(Memory const&) = delete;
	void operator=(Memory const&) = delete;

	void garbageCollect();
	void switchHeaps();

	static const int initial_heap_size = 10000;
	constexpr static const double scale = 2.0;

	int heap_size;
	char* heap_ptr;
	char* heap_begin;
	char* heap_end;
	char* copy_heap_begin;

	enum {GROW, SHRINK, STAY} grow_heap;

	Reader* reader;
	Evaluator* evaluator;
};

#endif
