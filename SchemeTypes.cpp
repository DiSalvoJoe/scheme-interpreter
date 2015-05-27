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
