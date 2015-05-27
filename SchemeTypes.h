#ifndef SCHEMETYPES_H
#define SCHEMETYPES_H

typedef char* symbol;
class Environment;
class Evaluator;

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
	ENVIRONMENT = 10,
    MACRO = 11
};

struct Object;

typedef struct {
    Object* car;
    Object* cdr;
} ConsCell;

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
	};

};
typedef struct Object Object;

struct Frame;
typedef struct Frame* (*FrameProcedure)(Evaluator& evaluator);
struct Frame {
    Object* to_eval;
    Object* result;
    struct Frame* return_frame;
    Environment* env;
    FrameProcedure cont;
    bool receive_return_as_list;
};

typedef struct Frame Frame;


void setFrame(Frame* frame, Object* to_eval, Object* result, Frame* ret, Environment* env, FrameProcedure cont, bool as_list);
Object* reverseList(Object* obj);
bool isSelfEvaluating(Object* obj);
bool asBool(Object* obj);

#endif
