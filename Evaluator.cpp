#include "Evaluator.h"
#include "Memory.h"
#include "SchemeTypes.h"
#include <iostream>

// Might trigger GC
void Evaluator::sendReturn() {
    Frame* from = top_frame;
    Frame* to = from->return_frame;
    if (from->receive_return_as_list) {
        from->result = reverseList(from->result);
    }
    if (to->receive_return_as_list) {
        Object* link = (Object*)memory.getBytes(sizeof(Object));
        // if GC occurs, the from and to pointers will be invalid. Need to reassign them.
        Frame* from = top_frame;
        Frame* to = from->return_frame;
        link->type = CONS;
        link->cell.cdr = to->result;
        link->cell.car = from->result;
        to->result = link;
    } else {
        to->result = from->result;
    }
    top_frame = to;
}

void dispatchEval(Evaluator& evaluator);
void selectIf(Evaluator& evaluator);

// Evaluate the predicate.
void beginIf(Evaluator& evaluator) {
    Frame* predicate_frame = (Frame*)evaluator.memory.getBytes(sizeof(Frame));
    Frame* top = evaluator.top_frame;
    setFrame(predicate_frame, top->to_eval->cell.car, nullptr, top, top->env, &dispatchEval, false); 
    top->to_eval = top->to_eval->cell.cdr;
    top->cont = &selectIf; // continue the 'if' evaluation after evaluating the predicate
    evaluator.top_frame = predicate_frame;
}

// if the predicate is true, evaluate the consequent (car), otherwise evaluate the alternative (cadr)
void selectIf(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    top->to_eval = asBool(top->result) ? (top->to_eval->cell.car) : (top->to_eval->cell.cdr->cell.car);
    top->result = nullptr;
    top->cont = &dispatchEval; // for tail call optimization, update this frame instead of creating a new one 
}


void doBind(Evaluator& evaluator);
// Evaluate the rvalue
void beginBind(Evaluator& evaluator) {
    Frame* eval_rvalue_frame = (Frame*)evaluator.memory.getBytes(sizeof(Frame));
    Frame* top = evaluator.top_frame;
    Object* rvalue = top->to_eval->cell.cdr->cell.car;
    setFrame(eval_rvalue_frame, rvalue, nullptr, top, top->env, &dispatchEval, false);
    top->to_eval = top->to_eval->cell.car; // save the lvalue in to_eval
    top->cont = &doBind; 
    evaluator.top_frame = eval_rvalue_frame;
}

// Bind the lvalue (car of to_eval) to the rvalue (result), return the rvalue
void doBind(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    top->env->bind(top->to_eval->sym, top->result);
    evaluator.sendReturn();
}

// to_eval is a list of things to evaluate in sequence
void evalSequence(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    // if there is nothing left to eval, return. this frame's result contains the evaluation of
    // the last expression in the sequence (or null, if there was no sequence).
    if (!top->to_eval) {
        evaluator.sendReturn();
    } 
    // if this is the last in the sequence, update this frame to eval it for tail call optimization
    else if (! top->to_eval->cell.cdr) {
        Frame* top = evaluator.top_frame;
        top->to_eval = top->to_eval->cell.car;
        top->cont = &dispatchEval;
    } 
    // otherwise, evaluate the first and set to_eval to be the rest of the sequence.
    else {  
        Frame* eval_car_frame = (Frame*)evaluator.memory.getBytes(sizeof(Frame));
        Frame* top = evaluator.top_frame;
        Object* car = top->to_eval->cell.car;
        top->to_eval = top->to_eval->cell.cdr;
        setFrame(eval_car_frame, car, nullptr, top, top->env, &dispatchEval, false);
        evaluator.top_frame = eval_car_frame;
    }
}

void evalList(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    if (!top->to_eval) {
        evaluator.sendReturn();
    } else {
        top->receive_return_as_list = true;
        Frame* eval_car_frame = (Frame*)evaluator.memory.getBytes(sizeof(Frame));
        Frame* top = evaluator.top_frame; // in case GC occurred
        Object* car = top->to_eval->cell.car;
        top->to_eval = top->to_eval->cell.cdr;
        setFrame(eval_car_frame, car, nullptr, top, top->env, &dispatchEval, false);
        evaluator.top_frame = eval_car_frame;
    }
}

void selectApply(Evaluator& evaluator);
void applyClosure(Evaluator& evaluator);

// Evaluate the procedure (car of to_eval) and store it in result
void beginApply(Evaluator& evaluator) {
    Frame* eval_proc_frame = (Frame*)evaluator.memory.getBytes(sizeof(Frame));
    Frame* top = evaluator.top_frame;
    setFrame(eval_proc_frame, top->to_eval->cell.car,nullptr, top, top->env, &dispatchEval, false);
    top->to_eval = top->to_eval->cell.cdr;
    top->cont = &selectApply;
    evaluator.top_frame = eval_proc_frame;
}

void selectApply(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    Object* proc = top->result;
    if (proc->type != MACRO) {
        // evaluate the arguments, then move to application
        top->receive_return_as_list = true; // result will be a pair, car is evalled args and cdr is the proc
        Frame* eval_args = (Frame*)evaluator.memory.getBytes(sizeof(Frame));
        Frame* top = evaluator.top_frame; // in case GC occurred
        setFrame(eval_args, top->to_eval, nullptr, top, top->env, &evalList, true);
        if (proc->type == CLOSURE) {
            top->cont = &applyClosure;
        } else if (proc->type == PRIM_PROC) {

        } else if (proc->type == CONTINUATION) {

        } else {

        }
        evaluator.top_frame = eval_args;
    } else {

        // macro eval: apply macro proc to unevaluated arguments, then call dispatchEval on it.
    }
}

void applyClosure(Evaluator& evaluator) {
    Environment* new_env = new Environment();  // gets the bytes from the main memory
    Frame* top = evaluator.top_frame;
    Closure* closure = top->result->cell.cdr->closure;
    Object* args = top->result->cell.car;

    // create a new environment with this environment enclosing it
    new_env->enclosing_env = top->env;

    // bind the parameters to the evaluated arguments
    Object* params = closure->parameters;
    while (params && args) {
        symbol identifier = params->cell.car->sym;
        Object* rvalue = args->cell.car;
        new_env->bind(identifier, rvalue);
        params = params->cell.cdr;
        args = args->cell.cdr;
    }

    // evaluate the body sequence in the new environment
    top->result = nullptr;
    top->receive_return_as_list = false;
    top->env = new_env;
    top->to_eval = closure->body;
    top->cont = &evalSequence;
}

void lambdaToClosure(Evaluator& evaluator) {
    // get a closure and a wrapper (object) to put it in, return it.
    Object* closure_obj = (Object*)evaluator.memory.getBytes(sizeof(Object)+sizeof(Closure));
    Closure* closure = (Closure*)(closure_obj+sizeof(Object));
    closure_obj->type = CLOSURE;
    closure_obj->closure = closure;
    closure->env = evaluator.top_frame->env;
    Object* list = evaluator.top_frame->to_eval;
    closure->parameters = list->cell.car;
    closure->body = list->cell.cdr;
    evaluator.top_frame->result = closure_obj;
    evaluator.sendReturn();
}

void dispatchEval(Evaluator& evaluator) {
    Frame* frame = evaluator.top_frame;
    Object* to_eval = frame->to_eval;
    if (isSelfEvaluating(to_eval)) {
        frame->result = to_eval;
        evaluator.sendReturn();
    } else {
        if (to_eval->type == SYMBOL) {
            frame->result = frame->env->lookup(to_eval->sym);
            evaluator.sendReturn();
        } 
        
        // this should be the case
        else if (to_eval->type == CONS) {
            symbol car = to_eval->cell.car->sym;

            if (car == evaluator.quote_sym) {
                frame->result = to_eval->cell.cdr->cell.car;
                evaluator.sendReturn();
            }

            else if (car == evaluator.if_sym) {
                frame->receive_return_as_list = false;
                frame->cont = &beginIf;
                frame->to_eval = to_eval->cell.cdr;
            }

            else if (car == evaluator.define_sym || car == evaluator.set_sym) {
                frame->receive_return_as_list = false;
                frame->cont = &beginBind;
                frame->to_eval = to_eval->cell.cdr;
            }

            else if (car == evaluator.begin_sym) {
                frame->receive_return_as_list = false;
                frame->cont = &evalSequence;
                frame->to_eval = to_eval->cell.cdr;
            }

            else if (car == evaluator.lambda_sym) {
                frame->receive_return_as_list = false;
                frame->cont = &lambdaToClosure;
                frame->to_eval = to_eval->cell.cdr;
            }

            // applying a procedure to an object.
            else {
                frame->receive_return_as_list = false;
                frame->cont = &beginApply;
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


Object* Evaluator::eval(Object* obj, Environment* env) {
    final_frame = (Frame*)memory.getBytes(sizeof(Frame));
    top_frame = (Frame*)memory.getBytes(sizeof(Frame));
    setFrame(final_frame, nullptr, nullptr, nullptr, nullptr, nullptr, false);
    setFrame(top_frame, obj, nullptr, final_frame, env, &dispatchEval, false);

    while (!final_frame->result) {
        top_frame->cont(*this);
    }

    Object* result = final_frame->result;
    final_frame = nullptr;
    top_frame = nullptr;
    return result;
}
