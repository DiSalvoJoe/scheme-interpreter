#include "SchemeTypes.h"
#include "Memory.h"
#include <iostream>

void* Environment::operator new(size_t size) {
    Memory& mem = Memory::getTheMemory();
    return (void*)mem.getBytes(size);
}
void Environment::operator delete(void* p) {
    free(p);
}
Environment::Environment() {
    enclosing_env = nullptr;
}

Object* Environment::lookup(symbol identifier) {
    auto found = table.find(identifier);
    if (found == table.end()) {
        if (enclosing_env) {
            return enclosing_env->lookup(identifier);
        } else {
            return nullptr;
        }
    } else {
        return found->second;
    }
}

void Environment::bind(symbol identifier, Object* rvalue) {
    table.insert(std::pair<symbol, Object*>(identifier, rvalue));
}


Object* reverseList(Object* obj) {
    if (obj == nullptr || obj->type != CONS) {
        return obj;
    }
    Object* prev = nullptr;
    Object* cur = obj;
    Object* next = obj->cell.cdr;
    while (next) {
        cur->cell.cdr = prev;
        prev = cur;
        cur = next;
        next = next->cell.cdr;
    }
    cur->cell.cdr = prev;
    return cur;
}

bool isSelfEvaluating(Object* obj) {
    if (obj == nullptr) {
        return true;
    }

    switch (obj->type) {
        case SYMBOL:
        case CONS:
            return false;
        default:
            return true;
    }
}

bool asBool(Object* obj) {
    if (obj && obj->type == BOOL) {
        return obj->boolean;
    } else {
        return true;
    }
}

bool equal(Object* left, Object* right) {
    if (!left || !right) {
        return !right && !left;
    } else if (left->type != right->type) {
        return false;
    } else {
        switch (left->type) {
            case INT:
                return left->integer == right->integer;
            case FLOAT:
                return left->floatN == right->floatN;
            case CHAR:
                return left->character == right->character;
            case BOOL:
                return left->boolean == right->boolean;
            case STRING:
                return strcmp(left->string, right->string) == 0;
            case SYMBOL:
                return left->sym == right->sym;
            case CONS:
                return equal(left->cell.car, right->cell.car) && equal(left->cell.cdr, right->cell.cdr);
            default:
                // There should be no default, but I haven't implemented continuations and macros etc. yet.
                return false;
        }
    }
}

        

void setFrame(Frame* frame, Object* to_eval, Object* result, Frame* ret, Environment* env, FrameProcedure cont, bool as_list) {
    frame->to_eval = to_eval;
    frame->result = result;
    frame->return_frame = ret;
    frame->env = env;
    frame->cont = cont;
    frame->receive_return_as_list = as_list;
}

