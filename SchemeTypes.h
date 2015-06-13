#ifndef SCHEMETYPES_H
#define SCHEMETYPES_H

#include <map>
#include <cstring>
#include <ostream>

typedef char* symbol;
struct Environment;
class Evaluator;
struct Frame;
typedef void (*FrameProcedure)(Evaluator& evaluator);

enum MarkStatus {
	UNMARKED = 0,
	FORWARDED = 1
};

enum Type {
	INT = 0,
	FLOAT = 1,
	CHAR = 2,
	BOOL = 3,
	STRING = 4,
	SYMBOL = 5,
	CONS = 6,
	CONTINUATION = 7,
	CLOSURE = 8,
	PRIM_PROC = 9,
    MACRO = 10
};

struct Object;

typedef struct {
    Object* car;
    Object* cdr;
} ConsCell;

typedef struct {
    Object* parameters;
    Object* body;
    Environment* env;
} Closure;

struct Object {
	MarkStatus marked : 1;
	Type type : 4;
	union {
		int integer;
		float floatN;
		char character;
		bool boolean;
		char* string;
		symbol sym;
        ConsCell cell;
        Closure* closure;
        FrameProcedure prim_proc;
	};

};

typedef struct Object Object;

struct Frame {
    Object* to_eval;
    Object* result;
    struct Frame* return_frame;
    Environment* env;
    FrameProcedure cont;
    bool receive_return_as_list : 1;
    MarkStatus marked : 1;
};

typedef struct Frame Frame;

struct Environment {
    MarkStatus marked : 1;
    struct Environment* enclosing_env;
    Object* assoc_list;
};
typedef struct Environment Environment;

class GlobalEnvironment {
public:
    static GlobalEnvironment& getGlobalEnvironment();

    ~GlobalEnvironment() {}

    void bind(symbol identifier, Object* rvalue);

    Object* lookup(symbol identifier);

    void copyAll();

    void clear();

private:
    GlobalEnvironment();

	struct cmp_str  {
		bool operator()(char const *a, char const *b) {
			return std::strcmp(a, b) < 0;
		}
	};

    // I think this will leak memory, because environments are never properly destroyed.
    // A large chunk of memory which contains an std::map header is destroyed instead.
    // Change this to be a scheme association list?
	std::map<symbol, Object*, cmp_str> table;
};


void setFrame(Frame* frame, Object* to_eval, Object* result, Frame* ret, Environment* env, FrameProcedure cont, bool as_list);

Object* reverseList(Object* obj);

bool isSelfEvaluating(Object* obj);

bool asBool(Object* obj);

bool equal(Object* left, Object* right);

size_t size(Object* list);

Object* copy(Object* obj);

Environment* copy(Environment* env);

Object* lookup(Environment* env, symbol identifier);

void write(Object* obj, std::ostream& os);


#endif
