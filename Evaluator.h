#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "SchemeTypes.h"
#include "Memory.h"

class EvaluationProcedure {
public:
    // Call stack manipulations
    // Detailed comments are with the implementation
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
    static void getCar(Evaluator& evaluator);
    static void getCdr(Evaluator& evaluator);

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

    Object* eval(Object* obj);

    // Copy the call stack and every object in it into the new primary memory.
    // Used for garbage collection.
    void copyAll();

    ~Evaluator() {}

private:
    Evaluator();
    Evaluator(const Evaluator& other) = delete;
    Evaluator& operator=(const Evaluator& other) = delete;

    // Return top_frame's result-value to its return frame's result field.
    void sendReturn();

    // Bind top_frame->to_eval->sym to top_frame->result
    void bindToEvalSymToResult(Environment* env);

    // eval(obj, env) stores obj and env in these temporary fields before requesting
    // a new top_frame from the memory. Then, if GC occurs, these temp fields will be updated
    // (unlike the arguments passed to eval, which would go stale)
    Object* to_eval_temp;

    // the top of the call stack
    Frame* top_frame;

    Memory& memory;
    SymbolTable& symbol_table;

    // Symbols for the special forms. These are used in dispatchEval. There is little overhead
    // because symbol equality is pointer equality (i.e. integer equality)
    const symbol define_sym;
    const symbol set_sym;
    const symbol if_sym;
    const symbol quote_sym;
    const symbol lambda_sym;
    const symbol begin_sym;

    friend class EvaluationProcedure;
};


#endif
