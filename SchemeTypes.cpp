#include "SchemeTypes.h"
#include "Memory.h"
#include "Evaluator.h"
#include <iostream>
#include <assert.h>
#include <set>
#include <stack>


/* Global Environment */
GlobalEnvironment::GlobalEnvironment() {
    SymbolTable& symbol_table = SymbolTable::getSymbolTable();
    Memory& memory = Memory::getTheMemory();
    bind(symbol_table.stringToSymbol("+"), getPrimProc(memory, &EvaluationProcedure::add));
    bind(symbol_table.stringToSymbol("-"), getPrimProc(memory, &EvaluationProcedure::subtract));
    bind(symbol_table.stringToSymbol("*"), getPrimProc(memory, &EvaluationProcedure::multiply));
    bind(symbol_table.stringToSymbol("/"), getPrimProc(memory, &EvaluationProcedure::divide));
    bind(symbol_table.stringToSymbol("list"), getPrimProc(memory, &EvaluationProcedure::list));
    bind(symbol_table.stringToSymbol("cons"), getPrimProc(memory, &EvaluationProcedure::cons));
    bind(symbol_table.stringToSymbol("null?"), getPrimProc(memory, &EvaluationProcedure::isNull));
    bind(symbol_table.stringToSymbol("pair?"), getPrimProc(memory, &EvaluationProcedure::isPair));
    bind(symbol_table.stringToSymbol("eval"), getPrimProc(memory, &EvaluationProcedure::eval));
    bind(symbol_table.stringToSymbol("apply"), getPrimProc(memory, &EvaluationProcedure::apply));
    bind(symbol_table.stringToSymbol("parse"), getPrimProc(memory, &EvaluationProcedure::parse));
    bind(symbol_table.stringToSymbol("print"), getPrimProc(memory, &EvaluationProcedure::print));
    bind(symbol_table.stringToSymbol("="), getPrimProc(memory, &EvaluationProcedure::structuralEquality));
    bind(symbol_table.stringToSymbol("eq?"), getPrimProc(memory, &EvaluationProcedure::pointerEquality));
}

GlobalEnvironment& GlobalEnvironment::getGlobalEnvironment() {
    static GlobalEnvironment ge;
    return ge;
}

Object* GlobalEnvironment::lookup(symbol identifier) {
    auto found = table.find(identifier);
    if (found == table.end()) {
        return nullptr;
    } else {
        return found->second;
    }
}

void GlobalEnvironment::bind(symbol identifier, Object* rvalue) {
    table.insert(std::pair<symbol, Object*>(identifier, rvalue));
}

void GlobalEnvironment::copyAll() {
    //std::cout << "Starting to copy global environment. " << std::endl;
    for (auto pair : table) {
        //std::cout << "pair.second->type == " << pair.second->type << std::endl;
        pair.second = copy(pair.second);
    }
    //std::cout << "Finished copy of global environment. " << std::endl;
}

void GlobalEnvironment::clear() {
    table.clear();
}

/* Environments */
Environment* copy(Environment* env) {
    if (!env) {
        return nullptr;
    }
    if (env->marked == FORWARDED) {
        return env->enclosing_env;
    } else {
        Memory& memory = Memory::getTheMemory();
        Environment* copied = (Environment*)memory.getBytes(sizeof(Environment));
        copied->marked = UNMARKED;
        copied->enclosing_env = copy(env->enclosing_env); 
        copied->assoc_list = copy(env->assoc_list);
        env->enclosing_env = copied;
        env->marked = FORWARDED;
        return copied;
    }
}

Object* lookup(Environment* env, symbol identifier) {
    Environment* current = env;
    while (current) {
        Object* head = current->assoc_list;
        while (head) {
            if (head->cell.car->cell.car->sym == identifier) {
                return head->cell.car->cell.cdr;
            } else {
                head = head->cell.cdr;
            }
        }
        current = current->enclosing_env;
    } 

    GlobalEnvironment& ge = GlobalEnvironment::getGlobalEnvironment();
    return ge.lookup(identifier);
}


/* Functions on Objects */
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
    size_t result = 0;
    std::stack<Object*> to_count;
    Object* it;
    to_count.push(obj);
    while (!to_count.empty()) {
        it = to_count.top();
        to_count.pop();
        if (it) {
            switch (it->type) {
                case STRING:
                    result += strlen(obj->string);
                    break;
                case CONS:
                    result += sizeof(Object);
                    if (it && it->type == CONS) {
                        to_count.push(car(it));
                        to_count.push(cdr(it));
                    }
                    break;
                default:
                    result += sizeof(Object);
                    break;
            }
        }
    }
    return result;
}


Object* copy(Object* obj) {
    if (!obj ) {
        return obj;
    }

    Memory& memory = Memory::getTheMemory();
    Object* result = nullptr;
    Object* temp = nullptr;
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
            result = getSchemeString(memory, obj->string);
            obj->cell.car= result;
            obj->marked = FORWARDED;
            return result;
        case SYMBOL:
            result = getObject(memory, SYMBOL);
            result->sym = obj->sym;
            return result;
        case CONS:
            if (obj->marked == FORWARDED) {
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
            if (obj->marked == FORWARDED) {
                return obj->cell.car; 
            }
            result = getClosure(memory);
            result->closure->body = copy(obj->closure->body);
            result->closure->parameters = copy(obj->closure->parameters);
            result->closure->env = copy(obj->closure->env);
            obj->cell.car = result;
            obj->marked = FORWARDED;
            return result;
        case PRIM_PROC:
            result = getPrimProc(memory, obj->prim_proc);
            return result;
    }
    return result;

}

void write(Object* obj, std::ostream& os) {
    if (!obj) {
        os << "()" << std::endl;
        return;
    }
    switch (obj->type) {
        case INT:
            os << obj->integer;
            break;
        case FLOAT:
            os << obj->floatN; 
            break;
        case CONS:
            os << "( ";
            /*while (obj && obj->type == CONS) {
                write(car(obj), os);
                os << " ";
                obj = cdr(obj);
            }*/
        default:
            return;
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

Frame* copy(Frame* frame) {
    if (!frame) {
        return nullptr;
    }
    if (frame->marked == FORWARDED) {
        return frame->return_frame;
    } else {
        Memory& memory = Memory::getTheMemory();
        frame->marked = FORWARDED;
        Frame* new_frame = getFrame(memory);
        Frame* ret = frame->return_frame;
        frame->return_frame = new_frame;
        setFrame(new_frame, copy(frame->to_eval), copy(frame->result), copy(ret), copy(frame->env), frame->cont,
                frame->receive_return_as_list);
        return new_frame;
    }
}

