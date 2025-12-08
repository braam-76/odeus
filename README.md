# Odeus LISP

Odeus Lisp language implementation in progress.

## BIG CHANGES

(Dec. 8, 2025): Now it can do something!

### How it works

Lexer is just stream of Token's. `lexer_next_token (Lexer* lexer)` used to get next token.

Parser creates AST_Node with top-level CONS pair, where CAR stores environment variables and CDR stores actual program wrapped in `BEGIN` function (explanation below). 

Evaluator evaluates as follows:

- If it is ATOM (symbol, integer, float, string, nil, quote), returns itself

- If it is CONS, looks for any matching symbol in if-else statement and evaluates that branch (bulky, but something, will change it so it will look in Environment). If it is not predefined function, tries to look up for symbol in Environment. Lastly, if it is not symbol, or symbol can't be found in Environment, error occurs.
    
### Notice

Notice than it is quickly created evaluator. It is bulky and not optimal. It is not even 0.0.1 version, I have a lot to do with my project to actually make it even callable as version 0.0.1 

### Predefined functions

- BEGIN: takes multiple expression, evaluates them and returns result of last expression 
- DEFINE: defines symbol and stores it in environment
- PRINT: prints value as AST_Node (but evaluates SYMBOL, gets its value from environment)
- QUOTE: quotes expression without evaluating
- Basic algebra (+, -, *, /): you would know what it does :D


## Project Structure

```
.
├── repl/          # REPL file (just to try code, will make it fullblown REPL, like other LISP's)
├── core/          # Core interpreter files
├── tests/         # Unit tests
├── CMakeLists.txt # Build system
├── .clang-format  # Formatting config (based on GNU styleguide)
└── README.md      # Project documentation
```

## Status

Work in progress. 

## Roadmap

- [x] Lexer (Nov. 9, 2025)
- [x] Parser (Nov. 12, 2025)
- [x] Interpreter (Dec. 8, 2025)
- [ ] Standard library
- [ ] REPL

## Contributing

Contributions and feedback are welcome. Open an issue or submit a pull request.

## License

See [LICENSE](LICENSE)
