#include "SchemeTypes.h"

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
    if (!obj) {
        return false;
    } else if (obj->type == BOOL) {
        return obj->boolean;
    } else {
        return true;
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
