#include "Memory.h"
#include <map>
#include <cstring>
#include <cstdlib>

Object* Heap::getSchemeString(const char* str) {
    int str_len = strlen(str);
    char* alloc = getBytes(str_len + sizeof(Object) + 1);
    strcpy(alloc, str);
    alloc[str_len] = '\0';
    Object* scheme_obj = (Object*)(alloc + str_len + 1);
    scheme_obj->type = STRING;
    scheme_obj->string = alloc;
    return scheme_obj;
}


ChunkHeap::ChunkHeap() {
    addChunk();
    current_chunk = 0;
}

ChunkHeap::~ChunkHeap() {
    for (chunk c : chunks) {
        delete[] c.block;
    }
}

void ChunkHeap::addChunk(int size) {
    chunk c;
    c.block = new char[size];
    c.block_index = 0;
    c.size = size;
    chunks.push_back(c);
    ++current_chunk;
}

char* ChunkHeap::getBytes(size_t bytes) {
    char* ptr;
    chunk& c = chunks[current_chunk];
    if ((c.block_index + (int)bytes) >= c.size) {
        addChunk((bytes > chunk_size) ? bytes : chunk_size);
        c = chunks[current_chunk];
    }
    ptr = &c.block[c.block_index];
    c.block_index += bytes;
    return ptr;
}

void ChunkHeap::ungetBytes(size_t bytes) {
    if (chunks[current_chunk].block_index - bytes < 0) {
        delete[] chunks[current_chunk].block;
        chunks.pop_back();
        --current_chunk;
        chunks[current_chunk].block_index = chunks[current_chunk].size-1;
    }
    chunks[current_chunk].block_index -= bytes;
}
Memory& Memory::getTheMemory() {
	static Memory theMemory;
	return theMemory;
}



Memory::Memory() {
	heap_size = initial_heap_size;
	heap_begin = new char[heap_size];
	heap_end = heap_begin+heap_size;
	heap_ptr = heap_begin;
	copy_heap_begin = nullptr;
	grow_heap = STAY;
}

void Memory::garbageCollect(size_t ensure_bytes) {
	switchHeaps();
	// Start with, Evaluator
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

Memory::~Memory() {
	delete[] heap_begin;
	delete[] copy_heap_begin;
}


char* Memory::getBytes(size_t bytes) {
	if ((heap_ptr + bytes) >= heap_end) {
		garbageCollect(bytes);
	}
	char* addr = heap_ptr;
	heap_ptr += bytes;
	return addr;
}

SymbolTable& SymbolTable::getSymbolTable() {
	static SymbolTable symbol_table;
	return symbol_table;
}


SymbolTable::SymbolTable() {}
SymbolTable::~SymbolTable() {
	/*for (std::pair<char*, symbol> entry : table) {
		delete[] entry.second;
	}*/
}

symbol SymbolTable::stringToSymbol(const char* string_start, int str_len) {
	if (string_start == nullptr) {
		return nullptr;
	}

	symbol sym;
	char* string = storage.getBytes(str_len+1);
	string[str_len] = '\0';
	strncpy(string, string_start, str_len);

	auto location = table.find(string);
	if (location == table.end()) {
		table.insert(std::pair<char*, symbol>(string, string));
		sym = string;
	} else {
		sym = location->second;
        storage.ungetBytes(str_len+1);
	}
	return sym;
}

symbol SymbolTable::stringToSymbol(const char* string) {
	return stringToSymbol(string, strlen(string));
}

char* SymbolTable::symbolToString(symbol sym) const {
	return sym;
}

