#include "Evaluator.h"
#include "Memory.h"
#include "SchemeTypes.h"
#include "Reader.h"

#include <iostream>
#include <stdexcept>
#include <string>

#define REQUIRE(p, str) if (!(p)) {throw std::logic_error((str));} 

/* The current state of evaluation is stored in the top frame's 
 "cont" field, which a pointer to a function.
 Each cont manipulates the top frame and creates at most one additional frame.
 It would be easier to write the evaluation procedures recursively, instead of
 hardcoding the call stack, but there are several issues with that:
      -- Garbage collection can occur after any allocation, so temporary objects
         higher on the C++ call stack will become stale after GC, and there is no way
         to traverse the C++ call stack to update these temporary objects. With a custom
         call stack, every temporary variable is held in some field of a stack frame,
         and can be easily updated.
     --  Tail call optimization is easier. Instead of creating new stack frames, update the 
         existing one's continuation procedure instead.
     --  Continuations require doing a deep copy of the call stack.
 */
Object* Evaluator::eval(Object* obj, Environment* env) {
    // To avoid stale pointers after GC, use these temp objects
    to_eval_temp = obj;
    env_temp = env;
    // create the top_frame and initialize it
    top_frame = getFrame(memory);
    setFrame(top_frame, to_eval_temp, nullptr, nullptr, env_temp, &EvaluationProcedure::dispatchEval, false);
    to_eval_temp = nullptr;
    env_temp = nullptr;

    // transform the stack
    while (top_frame->cont) {
        top_frame->cont(*this);
    }

    Object* result = top_frame->result;
    top_frame = nullptr;
    return result;
}

// copyAll for garbage collection
void Evaluator::copyAll() {
    to_eval_temp = copy(to_eval_temp);
    env_temp = copy(env_temp);
    top_frame = copy(top_frame);
}


// Might trigger GC
// Send the top frame's result to its return frame.
void Evaluator::sendReturn() {
    Frame* from = top_frame;
    Frame* to = from->return_frame;
    if (!to) {
        from->cont = nullptr;
        return;
    }
    if (from->receive_return_as_list) {
        from->result = reverseList(from->result);
    }
    if (to->receive_return_as_list) {
        Object* link = getObject(memory, CONS);
        // if GC occurs, the from and to pointers will be invalid. Need to reassign them.
        from = top_frame;
        to = from->return_frame;
        cdr(link) = to->result;
        car(link) = from->result;
        to->result = link;
    } else {
        to->result = from->result;
    }
    top_frame = to;
}

// Evaluate the predicate.
// Precondition: to_eval = (<predicate> <consequent> <alternative>)
// Postcondition: selectIf with to_eval = (<consequent> <alternative>) and result = eval(<predicate>)
void EvaluationProcedure::beginIf(Evaluator& evaluator) {
    Frame* predicate_frame = getFrame(evaluator.memory);
    Frame* top = evaluator.top_frame;
    setFrame(predicate_frame, car(top->to_eval), nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false); 
    top->to_eval = cdr(top->to_eval);
    top->cont = &EvaluationProcedure::selectIf; // continue the 'if' evaluation after evaluating the predicate
    evaluator.top_frame = predicate_frame;
}

// if the predicate is true, evaluate the consequent (car), otherwise evaluate the alternative (cadr)
// Precondition: to_eval = (<consequent> <alternative>)
// Postcondition: dispatchEval on either <consequent> or <alternative>
void EvaluationProcedure::selectIf(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    top->to_eval = asBool(top->result) ? car(top->to_eval) : cadr(top->to_eval);
    top->result = nullptr;
    top->cont = &EvaluationProcedure::dispatchEval; // for tail call optimization, update this frame instead of creating a new one 
}


// Evaluate the rvalue
// Precondition: to_eval = (<identifier> <rvalue>) OR ( (<identifier> <param 1> ... <param n>) <body>)
// Postcondition: doBind with result as a closure or evalled rvalue and to_eval = lvalue
void EvaluationProcedure::beginDefine(Evaluator& evaluator) {
    Frame* eval_rvalue_frame = getFrame(evaluator.memory);
    Frame* top = evaluator.top_frame;
    Object* lvalue;
    // are we defining a function with the form (define (square x) (* x x))
    if (top->to_eval->cell.car->type == CONS) {
        lvalue = caar(top->to_eval);
        // transform to_eval into a lambda form, then get a closure from it
        car(top->to_eval)= cdar(top->to_eval); // parameter list
        setFrame(eval_rvalue_frame, top->to_eval, nullptr, top, top->env, &EvaluationProcedure::lambdaToClosure, false);
    } 
    // (<identifier> <rvalue>)
    else {
        lvalue = car(top->to_eval);
        Object* rvalue = cadr(top->to_eval);
        setFrame(eval_rvalue_frame, rvalue, nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
    }
    top->to_eval = lvalue; 
    top->cont = &EvaluationProcedure::doBind; 
    evaluator.top_frame = eval_rvalue_frame;
}

// Need to change this to update binding rather than add a new binding
void EvaluationProcedure::beginSet(Evaluator& evaluator) {
    Frame* eval_rvalue_frame = getFrame(evaluator.memory);
    Frame* top = evaluator.top_frame;
    Object* rvalue = cadr(top->to_eval);
    setFrame(eval_rvalue_frame, rvalue, nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
    top->to_eval = car(top->to_eval); // save the lvalue in to_eval
    top->cont = &EvaluationProcedure::doBind; 
    evaluator.top_frame = eval_rvalue_frame;
}

// Bind the lvalue (car of to_eval) to the rvalue (result), return the rvalue
void EvaluationProcedure::doBind(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    evaluator.bindToEvalSymToResult(top->env); 
    evaluator.sendReturn();
}

// Precondition: to_eval is a list of things to evaluate in sequence
// Postcondition: each item is evaluated in sequence, and the result of evaluating
// the last is returned to the caller.
void EvaluationProcedure::evalSequence(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    // if there is nothing left to eval, return. this frame's result contains the evaluation of
    // the last expression in the sequence (or null, if there was no sequence).
    if (!top->to_eval) {
        evaluator.sendReturn();
    } 
    // if this is the last in the sequence, update this frame to eval it for tail call optimization
    else if (! top->to_eval->cell.cdr) {
        Frame* top = evaluator.top_frame;
        top->to_eval = car(top->to_eval);
        top->cont = &EvaluationProcedure::dispatchEval;
    } 
    // otherwise, evaluate the first and set to_eval to be the rest of the sequence.
    else {  
        Frame* eval_car_frame = getFrame(evaluator.memory);
        Frame* top = evaluator.top_frame;
        Object* obj_car = car(top->to_eval);
        top->to_eval = cdr(top->to_eval);
        setFrame(eval_car_frame, obj_car, nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
        evaluator.top_frame = eval_car_frame;
    }
}

// Precondition: to_eval is a list of things to evaluation in sequence
// Postcondition: each item in the list is evaluated, and a new list consisting of
// these evaluated items is returned to the caller.
void EvaluationProcedure::evalList(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    if (!top->to_eval) {
        evaluator.sendReturn();
    } else {
        top->receive_return_as_list = true;
        Frame* eval_car_frame = getFrame(evaluator.memory);
        Frame* top = evaluator.top_frame; // in case GC occurred
        Object* obj_car = car(top->to_eval);
        top->to_eval = cdr(top->to_eval);
        setFrame(eval_car_frame, obj_car, nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
        evaluator.top_frame = eval_car_frame;
    }
}

// Evaluate the procedure (car of to_eval) and store it in result
// Precondition: to_eval = (<procedure> <arguments>)
// Postcondition: to_eval = <arguments> and result = eval(<procedure>) in selectApply frame
void EvaluationProcedure::beginApply(Evaluator& evaluator) {
    Frame* eval_proc_frame = getFrame(evaluator.memory);
    Frame* top = evaluator.top_frame;
    setFrame(eval_proc_frame, car(top->to_eval) ,nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
    top->to_eval = cdr(top->to_eval);
    top->cont = &EvaluationProcedure::selectApply;
    evaluator.top_frame = eval_proc_frame;
}

// Precondition: to_eval = <arguments> (list) and result = procedure object (closure, macro, prim proc, continuation)
// Postcondition: result = (evalList(<arguments>) . procedure) in applyClosure frame OR
//                result = evalList(<arguments>) in a primitive procedure's frame    OR
void EvaluationProcedure::selectApply(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    Object* proc = top->result;
    REQUIRE(proc, "Cannot apply null procedure in selectApply");
    if (proc->type != MACRO) {
        // evaluate the arguments, then move to application
        Frame* eval_args = getFrame(evaluator.memory);
        Frame* top = evaluator.top_frame; // in case GC occurred
        setFrame(eval_args, top->to_eval, nullptr, top, top->env, &evalList, true);
        if (proc->type == CLOSURE) {
            top->receive_return_as_list = true; // result will be a pair, car is evalled args and cdr is the proc
            top->cont = &EvaluationProcedure::applyClosure;
        } else if (proc->type == PRIM_PROC) {
            top->receive_return_as_list = false;
            top->cont = proc->prim_proc; // give control to the primitive procedure
            top->result = nullptr; // toss the prim_proc
        } else if (proc->type == CONTINUATION) {
            REQUIRE(false, "no continuation application yet");

        } else {
            std::cout << proc->type << std::endl;
            REQUIRE(false, "Can apply only closures, primitive procedures, macros, and continuations.");
        }
        evaluator.top_frame = eval_args;
    } else {
        REQUIRE(false, "no macro application yet");
        // macro eval: apply macro proc to unevaluated arguments, then call dispatchEval on it.
    }
}

// Precondition: result = (<evalled argument list> . <closure>)
// Postcondition: to_eval = body of closure
//                env = new env in which formal parameters of closure are bound to arguments
//                in an evalSequence frame 
void EvaluationProcedure::applyClosure(Evaluator& evaluator) {
    // need enough mem for a new environment, and assoc pairs for each argument
    char* bytes = evaluator.memory.getBytes(sizeof(Environment) + 3*sizeof(Object)*size(car(evaluator.top_frame->result)));

    Frame* top = evaluator.top_frame;
    Closure* closure = cdr(top->result)->closure;
    Object* args = car(top->result);
    Object* params = closure->parameters;
    Environment* new_env = (Environment*)bytes;
    bytes += sizeof(Environment);
    new_env->enclosing_env = closure->env;
    new_env->assoc_list = nullptr;
    new_env->marked = UNMARKED;

    // bind the parameters to the evaluated arguments
    while (params && args) {
        // need three objects: a symbol object for the identifier,
        // a pair object where car(pair ) = symbol object and cdr(pair) = argument
        // and an entry object where car(entry) = pair and cdr(entry) = rest of entries
        Object* entry = (Object*)bytes;
        bytes += sizeof(Object);
        Object* pair = (Object*)bytes;
        bytes+=sizeof(Object);
        Object* sym_obj = (Object*)bytes;
        bytes+=sizeof(Object);
        entry->type = CONS; pair->type = CONS; sym_obj->type = SYMBOL;
        entry->marked = UNMARKED; pair->marked = UNMARKED; sym_obj->marked= UNMARKED;
        car(entry)= pair;
        car(pair)= sym_obj;

        symbol identifier = car(params)->sym;
        Object* rvalue = car(args);
        car(pair)->sym = identifier;
        cdr(pair)= rvalue;
        cdr(entry)= new_env->assoc_list;
        new_env->assoc_list = entry;
        params = cdr(params);
        args = cdr(args);
    }

    // evaluate the body sequence in the new environment
    top->result = nullptr;
    top->receive_return_as_list = false;
    top->env = new_env;
    top->to_eval = closure->body;
    top->cont = &EvaluationProcedure::evalSequence;
}

// Precondition: to_eval = (<param list> <exp 1> ... < exp n>)
// Postcondition: send a corresponding closure to the return frame
void EvaluationProcedure::lambdaToClosure(Evaluator& evaluator) {
    // get a closure and a wrapper (object) to put it in, return it.
    Object* closure_obj = getClosure(evaluator.memory);
    closure_obj->closure->env = evaluator.top_frame->env;
    Object* list = evaluator.top_frame->to_eval;
    closure_obj->closure->parameters = car(list);
    closure_obj->closure->body = cdr(list);
    evaluator.top_frame->result = closure_obj;
    evaluator.top_frame->to_eval = nullptr;
    evaluator.sendReturn();
}

// Precondition: to_eval = general object
// Postcondition: the frame is a specific evaluation proceudre
void EvaluationProcedure::dispatchEval(Evaluator& evaluator) {
    Frame* frame = evaluator.top_frame;
    Object* to_eval = frame->to_eval;
    // Self evaluating objects (numbers, strings, etc.) are returned immediately
    if (isSelfEvaluating(to_eval)) {
        frame->result = to_eval;
        evaluator.sendReturn();
        return;
    } else {
        // Symbols are looked up in the environment
        if (to_eval->type == SYMBOL) {
            frame->result = lookup(frame->env, to_eval->sym); 
            if (!frame->result) {
                std::string str;
                str += "Symbol "; str += to_eval->sym; str += " is unbound.";
                REQUIRE(frame->result, str);
            }
            evaluator.sendReturn();
            return;
        } 
        
        // Otherwise, evaluating either a special form (if, begin, define, etc.)
        // or the application of a procedure to arguments.
        // Check all the special forms (symbol equality is pointer equality, so it's fast)
        // then default to procedure application
        else if (to_eval->type == CONS) {
            symbol car = car(to_eval)->sym; 

            // A quoted object is returned unevaluated
            if (car == evaluator.quote_sym) {
                frame->result = cadr(to_eval); 
                evaluator.sendReturn();
                return;
            }

            else if (car == evaluator.if_sym) {
                frame->cont = &EvaluationProcedure::beginIf;
                frame->to_eval = cdr(to_eval);
                return;
            }

            else if (car == evaluator.define_sym) {
                frame->cont = &EvaluationProcedure::beginDefine;
                frame->to_eval = cdr(to_eval);
                return;
            }
                
            else if (car == evaluator.set_sym) {
                frame->cont = &EvaluationProcedure::beginSet;
                frame->to_eval = cdr(to_eval);
                return;
            }

            else if (car == evaluator.begin_sym) {
                frame->cont = &EvaluationProcedure::evalSequence;
                frame->to_eval = cdr(to_eval);
                return;
            }

            else if (car == evaluator.lambda_sym) {
                frame->cont = &EvaluationProcedure::lambdaToClosure;
                frame->to_eval = cdr(to_eval);
                return;
            }

            // applying a procedure to an object.
            else {
                frame->cont = &EvaluationProcedure::beginApply;
            }
        }
    }
}

Evaluator::Evaluator() : 
    memory(Memory::getTheMemory()),
    symbol_table(SymbolTable::getSymbolTable()),
    define_sym(symbol_table.stringToSymbol("define")),
    set_sym(symbol_table.stringToSymbol("set!")),
    if_sym(symbol_table.stringToSymbol("if")),
    quote_sym(symbol_table.stringToSymbol("quote")),
    lambda_sym(symbol_table.stringToSymbol("lambda")),
    begin_sym(symbol_table.stringToSymbol("begin"))
{}

Evaluator& Evaluator::getEvaluator() {
    static Evaluator evaluator;
    return evaluator;
}


// Bind the top_frame->to_eval->sym to top_frame->result in the environment <env>
void Evaluator::bindToEvalSymToResult(Environment* env) {
    if (!env) {
        symbol identifier = top_frame->to_eval->sym;
        Object* rvalue = top_frame->result;
        GlobalEnvironment& ge = GlobalEnvironment::getGlobalEnvironment();
        ge.bind(identifier, rvalue);
        return;
    } else {
        Object* entry = getAssocListPair(memory);
        symbol identifier = top_frame->to_eval->sym;
        Object* rvalue = top_frame->result;
        entry->cell.car->cell.car->sym = identifier;
        entry->cell.car->cell.cdr = rvalue;
        entry->cell.cdr = env->assoc_list;
        env->assoc_list = entry;
    }
}

/* 
 * Primitive Procedures
 */

// ARITHMETIC
void EvaluationProcedure::add(Evaluator& evaluator) {
    Object* sum = getObject(evaluator.memory, INT);
    sum->integer = 0;
    bool as_int = true;
    Object* args = evaluator.top_frame->result;
    Object* num;
    while (args) {
        num = car(args);
        args = cdr(args);
        if (as_int) {
            if (num->type == FLOAT) {
                sum->type = FLOAT;
                sum->floatN = (float)sum->integer;
                sum->floatN += num->floatN;
                as_int = false;
            } else {
                sum->integer += num->integer;
            }
        } else {
            sum->floatN += (num->type == INT ? num->integer : num->floatN);
        }
    }
    evaluator.top_frame->result = sum;
    evaluator.sendReturn();
}

void EvaluationProcedure::subtract(Evaluator& evaluator) {
    Object* args = evaluator.top_frame->result;
    Object* num;
    args = args ? cdr(args) : args;
    while (args) {
        num = car(args);
        args = cdr(args);
        if (num->type == FLOAT) {
            num->floatN *= -1;
        } else {
            num->integer *= -1;
        }
    }
    evaluator.top_frame->cont = &EvaluationProcedure::add;
}

void EvaluationProcedure::divide(Evaluator& evaluator) {
    Object* product = getObject(evaluator.memory, INT);
    product->integer = 1;
    Object* args = evaluator.top_frame->result;
    Object* num;
    bool as_int = true;
    if (args) {
        num = car(args);
        args = cdr(args);
        if (num->type == INT) {
            product->integer = num->integer;
        } else {
            product->floatN = num->floatN;
            product->type= FLOAT;
            as_int = false;
        }
    }
        
    while (args) {
        num = car(args);
        args = cdr(args);
        if (as_int) {
            if (num->type == FLOAT) {
                product->type = FLOAT;
                product->floatN = (float)product->integer;
                product->floatN /= num->floatN;
                as_int = false;
            } else {
                product->integer /= num->integer;
            }
        } else {
            product->floatN /= (num->type == INT ? (float)num->integer : num->floatN);
        }
    }
    evaluator.top_frame->result = product;
    evaluator.sendReturn();
}

void EvaluationProcedure::multiply(Evaluator& evaluator) {
    Object* product = getObject(evaluator.memory, INT);
    product->integer = 1;
    bool as_int = true;
    Object* args = evaluator.top_frame->result;
    Object* num;
    while (args) {
        num = car(args);
        args = cdr(args);
        if (as_int) {
            if (num->type == FLOAT) {
                product->type = FLOAT;
                product->floatN = (float)product->integer;
                product->floatN *= num->floatN;
                as_int = false;
            } else {
                product->integer *= num->integer;
            }
        } else {
            product->floatN *= (num->type == INT ? num->integer : num->floatN);
        }
    }
    evaluator.top_frame->result = product;
    evaluator.sendReturn();
}

// Lists
void EvaluationProcedure::list(Evaluator& evaluator) {
    evaluator.sendReturn();
}

void EvaluationProcedure::cons(Evaluator& evaluator) {
    Object* args = evaluator.top_frame->result;
    cdr(args) = cadr(args);
    evaluator.sendReturn();
}

void EvaluationProcedure::argToBool(Evaluator& evaluator, bool (*predicate)(Object* obj)) {
    Object* boolean = getObject(evaluator.memory, BOOL);
    boolean->boolean = predicate(car(evaluator.top_frame->result));
    evaluator.top_frame->result = boolean;
    evaluator.sendReturn();
}

void EvaluationProcedure::isNull(Evaluator& evaluator) {
    argToBool(evaluator, [](Object* obj) -> bool {return obj == nullptr;});
}

void EvaluationProcedure::isPair(Evaluator& evaluator) {
    argToBool(evaluator, [](Object* obj) -> bool {return obj && (obj->type == CONS);});
}

// Meta
void EvaluationProcedure::eval(Evaluator& evaluator) {
    evaluator.top_frame->to_eval = car(evaluator.top_frame->result);
    evaluator.top_frame->result = nullptr; 
    evaluator.top_frame->cont = &EvaluationProcedure::dispatchEval;
}

void EvaluationProcedure::apply(Evaluator& evaluator) {
    Object* exp = evaluator.top_frame->result;
    Frame* top = evaluator.top_frame;
    top->result = nullptr;
    Object* proc = car(exp);
    Object* args = cadr(exp);
    if (proc->type == CLOSURE) {
        top->cont = &EvaluationProcedure::applyClosure;
        car(exp) = args;
        cdr(exp) = proc;
        top->result = exp;
    } else if (proc->type == PRIM_PROC) {
        top->cont = proc->prim_proc;
        top->result = args;
    } else {
        REQUIRE(false, "Primitive procedure 'apply' can apply only closures and primitive procedures.");
    }
}

void EvaluationProcedure::parse(Evaluator& evaluator) {
    Reader reader(car(evaluator.top_frame->result));
    Object* result = reader.read();
    evaluator.top_frame->result = result;
    evaluator.sendReturn();
}

void EvaluationProcedure::print(Evaluator& evaluator) {
    write(car(evaluator.top_frame->result), std::cout);
    evaluator.top_frame->result = nullptr;
    evaluator.sendReturn();
}

// Equality
void EvaluationProcedure::argsToBool(Evaluator& evaluator, bool (*predicate)(Object* l, Object* r)) {
    Object* boolean = getObject(evaluator.memory, BOOL);
    boolean->boolean = predicate(car(evaluator.top_frame->result), cadr(evaluator.top_frame->result));
    evaluator.top_frame->result = boolean;
    evaluator.sendReturn();
}

void EvaluationProcedure::structuralEquality(Evaluator& evaluator) {
    argsToBool(evaluator, equal);
}

void EvaluationProcedure::pointerEquality(Evaluator& evaluator) {
    argsToBool(evaluator, [](Object* l, Object* r) -> bool {return l == r;});
}


