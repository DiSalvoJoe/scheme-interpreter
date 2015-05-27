#include "Evaluator.h"
#include "Memory.h"
#include "SchemeTypes.h"

void Evaluator::sendReturn() {
    Frame* from = top_frame;
    Frame* to = from->return_frame;
    if (from->receive_return_as_list) {
        from->result = reverseList(from->result);
    }

    if (to->receive_return_as_list) {


    } else {
        to->result = from->result;
    }
}

Frame* dispatchEval(Evaluator& evaluator);
Frame* selectIf(Evaluator& evaluator);

Frame* beginIf(Evaluator& evaluator) {
    Frame* predicate_frame = (Frame*)evaluator.memory.getBytes(sizeof(Frame));
    Frame* top = evaluator.top_frame;
    setFrame(predicate_frame, top->to_eval->cell.car, nullptr, top, top->env, &dispatchEval, false); 
    top->to_eval = top->to_eval->cell.cdr;
    top->cont = &selectIf; // continue the 'if' evaluation after evaluating the predicate
    return predicate_frame;
}

Frame* selectIf(Evaluator& evaluator) {
    Frame* top = evaluator.top_frame;
    // if the predicate is true, evaluate the consequent (car), otherwise evaluate the alternative (cadr)
    top->to_eval = asBool(top->result) ? (top->to_eval->cell.car) : (top->to_eval->cell.cdr->cell.car);
    top->result = nullptr;
    top->cont = &dispatchEval; // for tail call optimization, update this frame instead of creating a new one 
    return top;
}


Frame* dispatchEval(Evaluator& evaluator) {
    Frame* frame = evaluator.top_frame;
    Object* to_eval = frame->to_eval;
    if (isSelfEvaluating(to_eval)) {
        frame->result = to_eval;
        evaluator.sendReturn();
        return frame->return_frame;
    } else {
        if (to_eval->type == SYMBOL) {
            // look it up in the environment

            // this should be the case
        } else if (to_eval->type == CONS) {
            symbol car = to_eval->cell.car->sym;
            if (car == evaluator.quote_sym) {
                frame->result = to_eval->cell.cdr->cell.car;
                evaluator.sendReturn();
                return evaluator.top_frame->return_frame;
            }
            if (car == evaluator.if_sym) {
                frame->receive_return_as_list = false;
                frame->cont = &beginIf;
                frame->to_eval = to_eval->cell.cdr;
                return frame;
            }
        }

        return frame;
    }
}

Evaluator::Evaluator() : 
    memory(Memory::getTheMemory()),
    symbol_table(SymbolTable::getSymbolTable()),
    define_sym(symbol_table.stringToSymbol("define")),
    if_sym(symbol_table.stringToSymbol("if")),
    quote_sym(symbol_table.stringToSymbol("quote"))
{}


Object* Evaluator::eval(Object* obj, Environment* env) {
    final_frame = getFrame(memory, nullptr, nullptr, nullptr, nullptr, nullptr, false);
    top_frame = getFrame(memory, obj, nullptr, final_frame, env, &dispatchEval, false);

    while (!final_frame->result) {
        top_frame = top_frame->cont(*this);
    }

    Object* result = final_frame->result;
    final_frame = nullptr;
    top_frame = nullptr;
    return result;
}
