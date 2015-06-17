#ifndef SCHEMETYPES_H
#define SCHEMETYPES_H

#include <map>
#include <cstring>
#include <ostream>

/* This defines the basic objects in Scheme (objects, closures, environments,
   call stack frames), and some functions that act on them.
*/

#define car(p) (p)->cell.car
#define cdr(p) (p)->cell.cdr
#define cadr(p) (p)->cell.cdr->cell.car
#define cdar(p) (p)->cell.car->cell.cdr
#define caar(p) (p)->cell.car->cell.car
#define cddr(p) (p)->cell.cdr->cell.cdr


typedef const char* symbol;
struct Environment;
class Evaluator;
struct Frame;
typedef void (*FrameProcedure)(Evaluator& evaluator);

// For stop and copy garbage collection.
enum MarkStatus {
	UNMARKED = 0,
	FORWARDED = 1
};

// Any object that the user can manipulate is of one of these types
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

// Call stack frames. A continuation is a pointer to a frame.
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

// Environments for closures. A map is used for the global environment,
// but association lists but from objects are used for smaller environments.
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

private:
    GlobalEnvironment();
    GlobalEnvironment(const GlobalEnvironment& other) = delete;
    GlobalEnvironment& operator=(const GlobalEnvironment& other) = delete;

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

bool eq(Object* left, Object* right);

size_t size(Object* list);

Object* copy(Object* obj);

Environment* copy(Environment* env);

Frame* copy(Frame* frame);

Object* lookup(Environment* env, symbol identifier);

void write(Object* obj, std::ostream& os);


#endif
