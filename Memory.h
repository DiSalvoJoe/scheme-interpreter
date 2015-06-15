#ifndef MEMORY_H
#define MEMORY_H

#include <map>
#include <cstring>
#include <vector>
#include <list>
#include "SchemeTypes.h"

/* This file defines two types of memory (Memory, ChunkHeap), the symbol table, and
   generic functions of the two types of memory for requesting objects, strings, closures etc.

   Memory should be used by processes that generate garbage (evaluation). It uses
   stop-and-copy garbage collection. It should be assumed that garbage collection can occur
   after any request for bytes from it.

   ChunkHeaps should be used by processes that don't generate garbage (the symbol table, parsing).
*/


// The following are generic functions on memory structures (Memory, ChunkHeap)
// Because the type of each memory is known at compile time, it's better to use templates
// than inheritance.
// Mem must support:
// Object* getBytes(size_t)
template <class Mem>
inline Object* getObject(Mem& memory, Type type) {
    Object* obj = (Object*)memory.getBytes(sizeof(Object));
    obj->marked = UNMARKED;
    obj->type = type;
    return obj;
}

template <class Mem>
inline Frame* getFrame(Mem& memory) {
    Frame* frame = (Frame*)memory.getBytes(sizeof(Frame));
    frame->marked = UNMARKED;
    return frame;
}

template <class Mem>
inline Object* getPrimProc(Mem& memory, FrameProcedure procedure) {
    Object* p = getObject(memory, PRIM_PROC);
    p->prim_proc = procedure;
    return p;
}

template <class Mem>
inline Object* getClosure(Mem& memory) {
    // If you do wrapper = (Object*)memory.getBytes ....
    // then get the closure by adding to wrapper, the wrapper address is offset
    // by sizeof(Object)*sizeof(Object) by pointer arithmetic.
    char* bytes= memory.getBytes(sizeof(Object)+sizeof(Closure));
    Object* wrapper = (Object*)bytes;
    Closure* cl = (Closure*)(bytes+ sizeof(Object));
    wrapper->type = CLOSURE;
    wrapper->marked = UNMARKED;
    wrapper->closure = cl;
    return wrapper;
}

template <class Mem>
Object* getSchemeString(Mem& mem, const char* str) {
    int str_len = strlen(str);
    char* alloc = mem.getBytes(str_len + sizeof(Object) + 1);
    strncpy(alloc, str, str_len);
    alloc[str_len] = '\0';
    Object* scheme_obj = (Object*)(alloc + str_len + 1);
    scheme_obj->type = STRING;
    scheme_obj->marked = UNMARKED;
    scheme_obj->string = alloc;
    return scheme_obj;
}

template <class Mem>
Object* getAssocListPair(Mem& mem) {
    char* bytes = mem.getBytes(sizeof(Object) * 3);
    Object* pair = (Object*)bytes;
    Object* entry = (Object*)(bytes+sizeof(Object));
    Object* sym_obj = (Object*)(bytes+sizeof(Object)*2);
    pair->type = CONS; entry->type = CONS; sym_obj->type = SYMBOL;
    pair->marked = UNMARKED; entry->marked = UNMARKED; sym_obj->marked= UNMARKED;
    entry->cell.car = pair;
    pair->cell.car = sym_obj;
    return entry;
}

template <class Mem>
Environment* getEnvironment(Mem& mem, Environment* enclosing_env) {
    Environment* env = (Environment*)mem.getBytes(sizeof(Environment));
    env->enclosing_env = enclosing_env;
    return env;
}

class ChunkHeap {
    public:
        ChunkHeap();
        ~ChunkHeap();
        char* getBytes(size_t bytes);
        void ungetBytes(size_t bytes);

    private:
        ChunkHeap(ChunkHeap const&) = delete;
        void operator=(ChunkHeap const&) = delete;

        static const int chunk_size = 1024;

        typedef struct chunk {
            char* block;
            int index;
            int size;
        } chunk;

        std::list<chunk*> chunks;

        void addChunk(int size = chunk_size);
};


class Memory {
public:
	static Memory& getTheMemory();
	~Memory();

	char* getBytes(size_t bytes);

    // Ensures that there are at least <bytes> bytes that can be requested before
    // garbage collection occurs. This is used internally, in getBytes, and by
    // the Reader, which gets the size of the parsed object, ensures the memory
    // has enough space for it, and copies it from a ChunkHeap to main memory.
    void requireBytes(size_t bytes);

private:

	Memory();

	Memory(Memory const&) = delete;
	void operator=(Memory const&) = delete;

    void garbageCollect(size_t ensure_bytes);

    // Make heap_ptr point to the copy_heap, increasing its size if necessary. 
    void switchHeaps(size_t ensure_bytes);
    void switchHeapPointers();

    // Copy everything in the global environment and evaluator (call stack) into the new
    // heap.
    void copyEverything();

    // Decide whether the heap should grow or stay the same size during the next GC.
    // Shrink the heap if less than 25% of it is being used.
    void changeGrowStatus();


    constexpr static const double scale = 2.0; // factor to expand heap by
    static const int initial_heap_size = 1000000;

    enum {GROW, STAY} grow_heap;
    int heap_size;
    char* heap_ptr; // everything between heap_begin and heap_ptr (exclusive) is in use;
    char* heap_begin;
    char* heap_end; // everything between heap_ptr (inclusive) and heap_end is free
    char* copy_heap_begin;

};

class SymbolTable {
public:
	 static SymbolTable& getSymbolTable();
	~SymbolTable();

    // Symbols are unique in memory. If the string has already been interned,
    // return its location; otherwise, create space for it in the table and return
    // the new address.
	symbol stringToSymbol(const char* string_start, int str_len);
	symbol stringToSymbol(const char* string);

    // Return the string of a symbol. 
	const char* symbolToString(symbol) const;

private:

	SymbolTable();
	SymbolTable(SymbolTable const&) = delete;
	void operator=(SymbolTable const&) = delete;

	struct cmp_str  {
		bool operator()(char const *a, char const *b) {
			return std::strcmp(a, b) < 0;
		}
	};

    ChunkHeap storage;

    // char* -> symbol is just for clarity; to insert str into the table, copy
    // str into the ChunkHeap storage (new_str), then create the entry table[new_str] = table[new_str]
	std::map<const char*, symbol, cmp_str> table; 

};

#endif
