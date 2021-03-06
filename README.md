Done:

1) Reader and Tokenizer 

1.5) Infix Cons for Reader and dotted-tail argument lists, like (define (add-up . ls) (apply + ls)), (add-up 1 2 3) = 6

1.75) Backquote and unquote, like `(1 ,(+ 5 7) (+ 1 2)) = (1 12 (+ 1 2))

2) Symbol Table and outline of Memory (although garbage collection isn't implemented, everything is written in a way that assumes garbage collection can occur after any allocation. Therefore, implementing garbage collection will be easy -- it will just involve adding functionality to Memory).

3) Evaluator, up through special forms (if, begin, define, lambda) and closures.

4) Change Environments so that they don't leak memory (either do mark-and-sweep for environments, so their std::maps can be deleted, or change environments to use a structure of scheme objects, so stop-and-copy works for them.)

5) Some Primitive Procedures (arithmetic, list, cons, eq?, =, parse, eval, apply)

6) Garbage Collection (there might be one or two stale pointer bugs)

7) Macros (defmacro)

To do:

1) Some Primitive procedures (>, <, string->symbol, symbol->string, call/cc) 

2) Continuations (because the evaluator's call stack is hardcoded, this shouldn't be too difficult. I just need to copy the call stack, which is more-or-less like copying a linked list).

3) Test Garbage Collection exhaustively

4) Rewrite copy functions to be more iterative to optimize GC.
