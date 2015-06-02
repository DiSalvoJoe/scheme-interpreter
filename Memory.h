#ifndef MEMORY_H
#define MEMORY_H

#include <map>
#include <cstring>
#include <vector>
#include "SchemeTypes.h"

typedef char* symbol;
class Reader;
class Evaluator;

class Heap {
    public:
        virtual ~Heap() {}
        virtual char* getBytes(size_t bytes) = 0;
        Object* getSchemeString(const char* str);
};

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

class ChunkHeap : public Heap {
    public:
        ChunkHeap();
        ~ChunkHeap();
        char* getBytes(size_t bytes);
        void ungetBytes(size_t bytes);

    private:
        static const int chunk_size = 64000; 

        void addChunk(int size = chunk_size);

        typedef struct chunk {
            char* block;
            int block_index;
            int size;
        } chunk;

        int current_chunk;
        std::vector<chunk> chunks;
};


class Memory : public Heap {
public:
	static Memory& getTheMemory();
	~Memory();

	char* getBytes(size_t bytes);
private:

	Memory();

	Memory(Memory const&) = delete;
	void operator=(Memory const&) = delete;

    void garbageCollect(size_t ensure_bytes);
    void switchHeaps();

    constexpr static const double scale = 2.0; // factor to expand heap by
    static const int initial_heap_size = 1000000;

    enum {GROW, SHRINK, STAY} grow_heap;
    int heap_size;
    char* heap_ptr;
    char* heap_begin;
    char* heap_end;
    char* copy_heap_begin;

	Evaluator* evaluator;
};

class SymbolTable {
public:
	 static SymbolTable& getSymbolTable();
	~SymbolTable();

	symbol stringToSymbol(const char* string_start, int str_len);
	symbol stringToSymbol(const char* string);
	char* symbolToString(symbol) const;

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
	std::map<char*, symbol, cmp_str> table;

};

#endif
