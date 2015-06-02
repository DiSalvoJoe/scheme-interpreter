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
        Object* link = getObject(memory, CONS);
        // if GC occurs, the from and to pointers will be invalid. Need to reassign them.
        from = top_frame;
        to = from->return_frame;
        link->cell.cdr = to->result;
        link->cell.car = from->result;
        to->result = link;
    } else {
        to->result = from->result;
    }
    top_frame = to;
}

// Evaluate the predicate.
void EvaluationProcedure::beginIf(Evaluator& evaluator) {
    Frame* predicate_frame = getFrame(evaluator.memory);
    Frame* top = evaluator.top_frame;
    setFrame(predicate_frame, top->to_eval->cell.car, nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false); 
    top->to_eval = top->to_eval->cell.cdr;
    top->cont = &EvaluationProcedure::selectIf; // continue the 'if' evaluation after evaluating the predicate
    evaluator.top_frame = predicate_frame;
}

// if the predicate is true, evaluate the consequent (car), otherwise evaluate the alternative (cadr)
void EvaluationProcedure::selectIf(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    top->to_eval = asBool(top->result) ? (top->to_eval->cell.car) : (top->to_eval->cell.cdr->cell.car);
    top->result = nullptr;
    top->cont = &EvaluationProcedure::dispatchEval; // for tail call optimization, update this frame instead of creating a new one 
}


// Evaluate the rvalue
void EvaluationProcedure::beginBind(Evaluator& evaluator) {
    Frame* eval_rvalue_frame = getFrame(evaluator.memory);
    Frame* top = evaluator.top_frame;
    Object* rvalue = top->to_eval->cell.cdr->cell.car;
    setFrame(eval_rvalue_frame, rvalue, nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
    top->to_eval = top->to_eval->cell.car; // save the lvalue in to_eval
    top->cont = &EvaluationProcedure::doBind; 
    evaluator.top_frame = eval_rvalue_frame;
}

// Bind the lvalue (car of to_eval) to the rvalue (result), return the rvalue
void EvaluationProcedure::doBind(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    top->env->bind(top->to_eval->sym, top->result);
    evaluator.sendReturn();
}

// to_eval is a list of things to evaluate in sequence
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
        top->to_eval = top->to_eval->cell.car;
        top->cont = &EvaluationProcedure::dispatchEval;
    } 
    // otherwise, evaluate the first and set to_eval to be the rest of the sequence.
    else {  
        Frame* eval_car_frame = getFrame(evaluator.memory);
        Frame* top = evaluator.top_frame;
        Object* car = top->to_eval->cell.car;
        top->to_eval = top->to_eval->cell.cdr;
        setFrame(eval_car_frame, car, nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
        evaluator.top_frame = eval_car_frame;
    }
}

void EvaluationProcedure::evalList(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    if (!top->to_eval) {
        evaluator.sendReturn();
    } else {
        top->receive_return_as_list = true;
        Frame* eval_car_frame = getFrame(evaluator.memory);
        Frame* top = evaluator.top_frame; // in case GC occurred
        Object* car = top->to_eval->cell.car;
        top->to_eval = top->to_eval->cell.cdr;
        setFrame(eval_car_frame, car, nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
        evaluator.top_frame = eval_car_frame;
    }
}

// Evaluate the procedure (car of to_eval) and store it in result
void EvaluationProcedure::beginApply(Evaluator& evaluator) {
    Frame* eval_proc_frame = getFrame(evaluator.memory);
    Frame* top = evaluator.top_frame;
    setFrame(eval_proc_frame, top->to_eval->cell.car,nullptr, top, top->env, &EvaluationProcedure::dispatchEval, false);
    top->to_eval = top->to_eval->cell.cdr;
    top->cont = &EvaluationProcedure::selectApply;
    evaluator.top_frame = eval_proc_frame;
}

void EvaluationProcedure::selectApply(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    Object* proc = top->result;
    if (proc->type != MACRO) {
        // evaluate the arguments, then move to application
        top->receive_return_as_list = true; // result will be a pair, car is evalled args and cdr is the proc
        Frame* eval_args = getFrame(evaluator.memory);
        Frame* top = evaluator.top_frame; // in case GC occurred
        setFrame(eval_args, top->to_eval, nullptr, top, top->env, &evalList, true);
        if (proc->type == CLOSURE) {
            top->cont = &EvaluationProcedure::applyClosure;
        } else if (proc->type == PRIM_PROC) {

        } else if (proc->type == CONTINUATION) {

        } else {

        }
        evaluator.top_frame = eval_args;
    } else {

        // macro eval: apply macro proc to unevaluated arguments, then call dispatchEval on it.
    }
}

void EvaluationProcedure::applyClosure(Evaluator& evaluator) {
    Environment* new_env = new Environment();  // gets the bytes from the main memory
    Frame* top = evaluator.top_frame;
    Closure* closure = top->result->cell.cdr->closure;
    Object* args = top->result->cell.car;

    // create a new environment with this environment enclosing it
    new_env->enclosing_env = top->env;

    // bind the parameters to the evaluated arguments
    // when I change the Environments to use scheme association lists, this next bit
    // will have to change, since binding with an assoc list might trigger GC.
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
    top->cont = &EvaluationProcedure::evalSequence;
}

void EvaluationProcedure::lambdaToClosure(Evaluator& evaluator) {
    // get a closure and a wrapper (object) to put it in, return it.
    Object* closure_obj = getClosure(evaluator.memory);
    closure_obj->closure->env = evaluator.top_frame->env;
    Object* list = evaluator.top_frame->to_eval;
    closure_obj->closure->parameters = list->cell.car;
    closure_obj->closure->body = list->cell.cdr;
    evaluator.top_frame->result = closure_obj;
    evaluator.top_frame->to_eval = nullptr;
    evaluator.sendReturn();
}

void EvaluationProcedure::dispatchEval(Evaluator& evaluator) {
    Frame* frame = evaluator.top_frame;
    Object* to_eval = frame->to_eval;
    if (isSelfEvaluating(to_eval)) {
        frame->result = to_eval;
        evaluator.sendReturn();
        return;
    } else {
        if (to_eval->type == SYMBOL) {
            frame->result = frame->env->lookup(to_eval->sym);
            evaluator.sendReturn();
            return;
        } 
        
        // this should be the case
        else if (to_eval->type == CONS) {
            symbol car = to_eval->cell.car->sym;

            if (car == evaluator.quote_sym) {
                frame->result = to_eval->cell.cdr->cell.car;
                evaluator.sendReturn();
                return;
            }

            else if (car == evaluator.if_sym) {
                frame->cont = &EvaluationProcedure::beginIf;
                frame->to_eval = to_eval->cell.cdr;
                return;
            }

            else if (car == evaluator.define_sym || car == evaluator.set_sym) {
                frame->cont = &EvaluationProcedure::beginBind;
                frame->to_eval = to_eval->cell.cdr;
                return;
            }

            else if (car == evaluator.begin_sym) {
                frame->cont = &EvaluationProcedure::evalSequence;
                frame->to_eval = to_eval->cell.cdr;
                return;
            }

            else if (car == evaluator.lambda_sym) {
                frame->cont = &EvaluationProcedure::lambdaToClosure;
                frame->to_eval = to_eval->cell.cdr;
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
{
    final_frame = getFrame(memory);
    top_frame = getFrame(memory);
}

Evaluator& Evaluator::getEvaluator() {
    static Evaluator evaluator;
    return evaluator;
}

void Evaluator::reinitialize() {
    final_frame = getFrame(memory);
    top_frame = getFrame(memory);
}


Object* Evaluator::eval(Object* obj, Environment* env) {
    setFrame(final_frame, nullptr, nullptr, nullptr, nullptr, nullptr, false);
    setFrame(top_frame, obj, nullptr, final_frame, env, &EvaluationProcedure::dispatchEval, false);

    while (!final_frame->result) {
        top_frame->cont(*this);
    }

    Object* result = final_frame->result;
    top_frame = nullptr;
    final_frame = nullptr;
    return result;
}
