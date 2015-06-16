#include "Memory.h"
#include "Evaluator.h"
#include "SchemeTypes.h"
#include <map>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <cmath>


inline void zeroMemory(char* mem, int mem_size) {
    for (int i = 0; i < mem_size; ++i) {
        mem[i] = 0;
    }
}

char* safeRealloc(char* array, size_t size) {
    delete[] array;
    array = new char[size];
    assert(array);
    return array;
}

ChunkHeap::ChunkHeap() {
    addChunk();
}

ChunkHeap::~ChunkHeap() {
    for (auto&& c : chunks) {
        delete[] c->block;
        delete c;
    }
    chunks.clear();
}

void ChunkHeap::addChunk(int size) {
    chunk* c = new chunk;
    c->block = new char[size];
    zeroMemory(c->block, size);
    c->index= 0;
    c->size = size;
    chunks.push_back(c);
}

char* ChunkHeap::getBytes(size_t bytes) {
    chunk* current_chunk = chunks.back();
    if ((current_chunk->index+ (int)bytes) >= current_chunk->size) {
        addChunk(bytes + chunk_size);
        current_chunk = chunks.back();
    }
    char* ptr = &current_chunk->block[current_chunk->index];
    current_chunk->index += bytes;
    return ptr;
}

void ChunkHeap::ungetBytes(size_t bytes) {
    chunk* current_chunk = chunks.back();
    if (current_chunk->index - (int)bytes < 0) {
        // just eat the loss
    } else {
        current_chunk->index -= (int)bytes;
    }
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
    std::cout << "Starting garbage collection." ;
	switchHeaps(ensure_bytes);
    copyEverything();
    changeGrowStatus();
    std:: cout << "  New Heap Size = " << heap_size << std::endl;
}

void Memory::copyEverything() {
    Evaluator& evaluator = Evaluator::getEvaluator();
    GlobalEnvironment& ge = GlobalEnvironment::getGlobalEnvironment();
    ge.copyAll();
    evaluator.copyAll();
}

void Memory::switchHeapPointers() {
	char* used_heap_begin = heap_begin;
	heap_begin = copy_heap_begin;
	heap_end = heap_begin + heap_size;
	heap_ptr = heap_begin;
	copy_heap_begin = used_heap_begin;
}

void Memory::switchHeaps(size_t ensure_bytes) {
    if (grow_heap == GROW) {
        heap_size *= scale;
    }
    heap_size += ensure_bytes;
    copy_heap_begin = safeRealloc(copy_heap_begin, heap_size);
    switchHeapPointers();
}

void Memory::changeGrowStatus() {
    // If less than 25% of the heap is used, Shrink it.
    // If more than 75% is used, Grow it.
    // Otherwise (26% - 74%), keep it the same.
    // These assume that scale = 2.
    float used = 1 - (heap_end - heap_ptr) / (float)heap_size;
    if (used < 0.25 && (0.25*heap_size > initial_heap_size)) {
        // shrink the copy heap
        grow_heap = STAY;
        heap_size /= scale;
        copy_heap_begin = safeRealloc(copy_heap_begin, heap_size);
        // switch the heaps and copy everything from old to new
        switchHeapPointers();
        copyEverything();
    } else if (used > 0.75) {
        grow_heap = GROW;
    } else {
        grow_heap = STAY;
    }
    copy_heap_begin = safeRealloc(copy_heap_begin, heap_size);
}


Memory::~Memory() {
	delete[] heap_begin;
	delete[] copy_heap_begin;
}


char* Memory::getBytes(size_t bytes) {
    requireBytes(bytes);
	char* addr = heap_ptr;
	heap_ptr += bytes;
	return addr;
}

void Memory::requireBytes(size_t bytes) {
	if ((heap_ptr + bytes) >= heap_end) {
		garbageCollect(bytes);
	}
}


SymbolTable& SymbolTable::getSymbolTable() {
	static SymbolTable symbol_table;
	return symbol_table;
}


SymbolTable::SymbolTable() {}
SymbolTable::~SymbolTable() {}

symbol SymbolTable::stringToSymbol(const char* string_start, int str_len) {
	if (string_start == nullptr) {
		return nullptr;
	}

	symbol sym;
	char* string = storage.getBytes(str_len+1);
	strncpy(string, string_start, str_len);
	string[str_len] = '\0';

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

const char* SymbolTable::symbolToString(symbol sym) const {
	return sym;
}

