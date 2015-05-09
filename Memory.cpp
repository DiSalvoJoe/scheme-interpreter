#include "Memory.h"
#include <map>
#include <cstring>
#include <cstdlib>

SymbolTable& SymbolTable::getSymbolTable() {
	static SymbolTable symbol_table;
	return symbol_table;
}


SymbolTable::SymbolTable() {}
SymbolTable::~SymbolTable() {
	for (std::pair<char*, symbol> entry : table) {
		delete[] entry.second;
	}
}

symbol SymbolTable::stringToSymbol(const char* string_start, int str_len) {
	if (string_start == nullptr) {
		return nullptr;
	}

	symbol sym;
	char* string = new char[str_len+1];
	string[str_len] = '\0';
	strncpy(string, string_start, str_len);

	auto location = table.find(string);
	if (location == table.end()) {
		table.insert(std::pair<char*, symbol>(string, string));
		sym = string;
	} else {
		sym = location->second;
		delete[] string;
	}
	return sym;
}

symbol SymbolTable::stringToSymbol(const char* string) {
	return stringToSymbol(string, strlen(string));
}

char* SymbolTable::symbolToString(symbol sym) const {
	return sym;
}

Memory::Memory(const int size) {
	heap_size = size;
	heap_begin = new char[heap_size];
	heap_end = heap_begin+size;
	heap_ptr = heap_begin;
	copy_heap_begin = nullptr;
	reader = nullptr;
	evaluator = nullptr;
	grow_heap = STAY;
}

Memory::~Memory() {
	delete[] heap_begin;
	delete[] copy_heap_begin;
}


Memory& Memory::getTheMemory() {
	static Memory theMemory(initial_heap_size);
	return theMemory;
}

void Memory::garbageCollect() {
	switchHeaps();
	// Start with Reader, Evaluator

	//....

}


/* NEED TO CHANGE THE OLD HEAP ALSO */
void Memory::switchHeaps() {
	char* used_heap_begin = heap_begin;
	switch (grow_heap) {
	case GROW:
		copy_heap_begin = (char*)realloc(copy_heap_begin, scale * heap_size);
		heap_size *= scale;
		break;
	case SHRINK:
		copy_heap_begin = (char*)realloc(copy_heap_begin, (1/scale) * heap_size);
		heap_size /= scale;
		break;
	case STAY:
		break;
	}
	heap_begin = copy_heap_begin;
	heap_end = heap_begin + heap_size;
	heap_ptr = heap_begin;
	copy_heap_begin = used_heap_begin;
}

char* Memory::getBytes(size_t bytes) {
	if ((heap_ptr + bytes) >= heap_end) {
		garbageCollect();
	}
	char* addr = heap_ptr;
	heap_ptr += bytes;
	return addr;
}
