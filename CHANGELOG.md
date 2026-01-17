## (Jan. 16, 2026): Now it somewhat usefull

Atleast this is what I think.

## (Dec. 14, 2025): Added lambda functions

Now Odeus LISP can do more useful stuff, by storing reused code in functions.
Yet it still does not have same semantics to define functions as in any Scheme LISP dialects. The reason is, Odeus LISP still does not have macros, quasiquotes, unquote and etc.

To define function in Odeus LISP, you just assign it to symbol

``` scheme
(define f (lambda (x) (+ x 4)))
; then use it as usually
(print (f 5))  ; => 9
```

Also, now program does not exit immediately, instead evaluator now returns AST.ERROR struct which has MESSAGE field with AST_ERROR type (! if error happend in eval step).
And both Lexer and Parser now has error field which stores place and message of error.

## (Dec. 8, 2025): Now it can do something!

### How it works

Lexer is just stream of Token's. `lexer_next_token (Lexer* lexer)` used to get next token.

Parser creates AST_Node with top-level CONS pair, where CAR stores environment variables and CDR stores actual program wrapped in `BEGIN` function (explanation below). 

Evaluator evaluates as follows:

- If it is ATOM (symbol, integer, float, string, nil, quote), returns itself

- If it is CONS, looks for any matching symbol in if-else statement and evaluates that branch (bulky, but something, will change it so it will look in Environment). If it is not predefined function, tries to look up for symbol in Environment. Lastly, if it is not symbol, or symbol can't be found in Environment, error occurs.
    
### Notice

than it is quickly created evaluator. It is bulky and not optimal. It is not even 0.0.1 version, I have a lot to do with my project to actually make it even callable as version 0.0.1.

And by the way, REPL exits right after error occurs (because whole code just exits with 1 on error) 

### Predefined functions

- BEGIN: takes multiple expression, evaluates them and returns result of last expression 
- DEFINE: defines symbol and stores it in environment
- PRINT: prints value as AST_Node (but evaluates SYMBOL, gets its value from environment)
- QUOTE: quotes expression without evaluating
- Basic algebra (+, -, *, /): you would know what it does :D
