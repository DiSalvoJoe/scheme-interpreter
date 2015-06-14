#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "SchemeTypes.h"
#include "Memory.h"

class EvaluationProcedure {
public:
    static void beginIf(Evaluator& evaluator);
    static void selectIf(Evaluator& evaluator);
    static void dispatchEval(Evaluator& evaluator);
    static void beginDefine(Evaluator& evaluator);
    static void beginSet(Evaluator& evaluator);
    static void doBind(Evaluator& evaluator);
    static void evalSequence(Evaluator& evaluator);
    static void evalList(Evaluator& evaluator);
    static void beginApply(Evaluator& evaluator);
    static void selectApply(Evaluator& evaluator);
    static void applyClosure(Evaluator& evaluator);
    static void lambdaToClosure(Evaluator& evaluator);

    // Primitive Procedures
    // Precondition: result = a list of evaluated arguments
    // Postcondition: primitive proceudre is applied to arguments and the result is
    //                returned to the caller.

    // Arithmetic:
    static void add(Evaluator& evaluator);
    static void subtract(Evaluator& evaluator);
    static void divide(Evaluator& evaluator);
    static void multiply(Evaluator& evaluator);

    // Lists:
    static void list(Evaluator& evaluator);
    static void cons(Evaluator& evaluator);
    static void isNull(Evaluator& evaluator);
    static void isPair(Evaluator& evaluator);

    // Meta:
    static void eval(Evaluator& evaluator);
    static void apply(Evaluator& evaluator);
    static void parse(Evaluator& evaluator);
    static void print(Evaluator& evaluator);
    static void callcc(Evaluator& evaluator);
    static void gc(Evaluator& evaluator);

    // Equality:
    static void structuralEquality(Evaluator& evaluator);
    static void pointerEquality(Evaluator& evaluator);
    static void lessThan(Evaluator& evaluator);
    static void greaterThan(Evaluator& evaluator);
    static void schemeNot(Evaluator& evaluator);

private:
    static void argToBool(Evaluator& evaluator, bool (*predicate)(Object* obj));
    static void argsToBool(Evaluator& evaluator, bool (*predicate)(Object* l, Object* r));
};

class Evaluator {
public:
    static Evaluator& getEvaluator();

    Object* eval(Object* obj, Environment* env);

    void copyAll();

    ~Evaluator() {}

private:
    Evaluator();

    void sendReturn();

    void bindToEvalSymToResult(Environment* env);

    void bindAll(Environment* env);

    Object* to_eval_temp;
    Environment* env_temp;

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
