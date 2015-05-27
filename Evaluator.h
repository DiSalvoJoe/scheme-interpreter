#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "SchemeTypes.h"
#include "Memory.h"


class Evaluator {
public:
    Evaluator();
    ~Evaluator() {}
    Object* eval(Object* obj, Environment* env);
    void sendReturn();

    Frame* final_frame;
    Frame* top_frame;

    Memory& memory;
    SymbolTable& symbol_table;
    const symbol define_sym;
    const symbol set_sym;
    const symbol if_sym;
    const symbol quote_sym;
    const symbol lambda_sym;
    const symbol begin_sym;
};

#endif
