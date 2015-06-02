#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "SchemeTypes.h"
#include "Memory.h"

class EvaluationProcedure {
public:
    static void beginIf(Evaluator& evaluator);
    static void selectIf(Evaluator& evaluator);
    static void dispatchEval(Evaluator& evaluator);
    static void beginBind(Evaluator& evaluator);
    static void doBind(Evaluator& evaluator);
    static void evalSequence(Evaluator& evaluator);
    static void evalList(Evaluator& evaluator);
    static void beginApply(Evaluator& evaluator);
    static void selectApply(Evaluator& evaluator);
    static void applyClosure(Evaluator& evaluator);
    static void lambdaToClosure(Evaluator& evaluator);
};

class Evaluator {
public:
    static Evaluator& getEvaluator();
    ~Evaluator() {}
    Object* eval(Object* obj, Environment* env);
    void reinitialize();

private:
    Evaluator();

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

    friend class EvaluationProcedure;
};


#endif
