#ifndef SCHEMETYPES_H
#define SCHEMETYPES_H

typedef char* symbol;

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

Object* reverseList(Object* obj);

#endif
