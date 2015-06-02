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
        return left == right;
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

size_t size(Object* obj) {
    if (!obj) {
        return 0;
    }

    switch (obj->type) {
        case STRING:
            return strlen(obj->string);
        case CONS:
            return sizeof(Object) + size(obj->cell.car) + size(obj->cell.cdr);
        default:
            return sizeof(Object);
    }
}

Environment* copyEnv(Environment* env);

Object* copy(Object* obj) {
    if (!obj ) {
        return obj;
    }

    Memory& memory = Memory::getTheMemory();
    Object* result = nullptr;
    Object* temp = nullptr;
    int len = 0;
    switch (obj->type) {
        case INT:
            result = getObject(memory, obj->type);
            result->integer = obj->integer;
            return result;
        case FLOAT:
            result = getObject(memory, obj->type);
            result->floatN = obj->floatN;
            return result;
        case CHAR:
            result = getObject(memory, obj->type);
            result->character = obj->character;
            return result;
        case BOOL:
            result = getObject(memory, obj->type);
            result->boolean = obj->boolean;
            return result;
        case STRING:
            if (obj->marked == FORWARDED) {
                return obj->cell.car; 
            }
            result = getObject(memory, obj->type);
            len = strlen(obj->string)+1;
            result->string = memory.getBytes(len);
            result->string[len-1] = '\0';
            strcpy(result->string, obj->string);
            obj->cell.car= result;
            obj->marked = FORWARDED;
            return result;
        case SYMBOL:
            result = getObject(memory, SYMBOL);
            result->sym = obj->sym;
            return result;
        case CONS:
            if (obj->marked == FORWARDED) {
                std::cout << "FORWARDED CONS!" << std::endl;
                return obj->cell.car; 
            }
            result = getObject(memory, CONS);
            temp = obj->cell.car;
            obj->cell.car = result;
            obj->marked = FORWARDED;
            result->cell.car = copy(temp);
            result->cell.cdr = copy(obj->cell.cdr);
            return result;
        case CONTINUATION:
            // to do
            break;
        case MACRO:
        case CLOSURE:
            std::cout << "Copying closure " << std::endl;
            if (obj->marked == FORWARDED) {
                return obj->cell.car; 
            }
            result = getObject(memory, obj->type);
            result->closure = (Closure*)memory.getBytes(sizeof(Closure));
            result->closure->body = copy(obj->closure->body);
            result->closure->parameters = copy(obj->closure->parameters);
            result->closure->env = copyEnv(obj->closure->env);
            obj->cell.car = result;
            obj->marked = FORWARDED;
            return result;
        case PRIM_PROC:
            result = getObject(memory, obj->type);
            result->prim_proc = obj->prim_proc;
            return result;
    }
    return result;

}

Environment* copyEnv(Environment* env) {
    return env;
}

        

void setFrame(Frame* frame, Object* to_eval, Object* result, Frame* ret, Environment* env, FrameProcedure cont, bool as_list) {
    frame->to_eval = to_eval;
    frame->result = result;
    frame->return_frame = ret;
    frame->env = env;
    frame->cont = cont;
    frame->receive_return_as_list = as_list;
}

