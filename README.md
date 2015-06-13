Done:

1) Reader and Tokenizer (except infix cons -- (1 . 2))

2) Symbol Table and outline of Memory (although garbage collection isn't implemented, everything is written in a way that assumes garbage collection can occur after any allocation. Therefore, implementing garbage collection will be easy -- it will just involve adding functionality to Memory).

3) Evaluator, up through special forms (if, begin, define, lambda) and closures.

4) Change Environments so that they don't leak memory (either do mark-and-sweep for environments, so their std::maps can be deleted, or change environments to use a structure of scheme objects, so stop-and-copy works for them.)


To do:

1) Primitive procedures (arithmetic, list, cons, eq?, =, >, <, etc. and read, eval, apply, call/cc)

2) Continuations (because the evaluator's call stack is hardcoded, this shouldn't be too difficult. I just need to copy the call stack, which is more-or-less like copying a linked list).

3) Garbage Collection (won't require any refactoring)

4) Macros (very easy -- apply macro's closure to unevaluated arguments, then evaluate the result).

5) Infix Cons for Reader and dotted-tail argument lists.

6) Rewrite copy functions to be iterative.
