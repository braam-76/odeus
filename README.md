# Odeus LISP

Odeus Lisp language implementation in progress. Currently, the project includes a lexer with tests.

## Features

* **Lexer:** Breaks down source code into tokens.
* **Unit Tests:** Tests written using [utest](https://github.com/alexcrichton/utest).
* **Build System:** Simple build using `Make`.

## Project Structure

```
.
├── core/         # Core interpreter files (for now only lexer)
├── tests/        # Unit tests (for now only lexer tests)
├── Makefile      # Build system
├── .clang-format # Formatting config (based on GNU styleguide)
└── README.md     # Project documentation
```

## Build and Test

To build the project and run tests:

```bash
make        # Actually dont run it, as it is not finished yet
make test   # Run lexer tests
```

## Status

⚠️ Work in progress. Only the lexer is implemented at the moment. And there is no final binary for interpreting code yet.

## Future Plans

* Parser
* Interpreter / Evaluation
* Standard library
* REPL

## Contributing

Contributions and feedback are welcome. Open an issue or submit a pull request.

## License

See [LICENSE](LICENSE)
