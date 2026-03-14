### Changelog

All notable changes to this project will be documented in this file. Dates are displayed in UTC.

This changelog is maintained manually based on the git history. The project is still in version `0.0.1`; entries are grouped by date rather than by tagged releases.

#### First steps

> 9 November 2025

- feat: Initial C codebase and basic Odeus LISP implementation.
- chore: Switched from a simple GNU Makefile to CMake for builds.
- feat: Added column tracking to lexer tokens for better error reporting.
- docs: Added `LICENSE` and the first version of `README.md`.

#### Parser arrives

> 12 November 2025

- feat: Introduced the first real parser with tests.
- refactor: Stripped module prefixes from lexer files and adjusted styles.
- chore: Minor visual and style tweaks around parsing and printing.

#### Lambda in the AST

> 16 November 2025

- feat: Added `LAMBDA` as an AST node, enabling first-class functions.
- refactor: Renamed `ATOM` to `SYMBOL` in the AST for clearer semantics.
- docs: Updated `README.md` checkboxes and wording.

#### Lexer & parser refinements

> 7 December 2025

- refactor: Updated lexer internals.
- refactor: Updated parser internals and cleaned up structure in preparation for evaluation.

#### First working evaluation

> 8 December 2025

- feat: Added helper functions and macros for CONS pairs.
- feat: Implemented the first working evaluator so Odeus can actually run programs.
- chore: Updated `CMakeLists.txt` to include evaluation and REPL sources.
- docs: Improved `README.md` to describe the new capabilities.

At this point Odeus LISP could:

- Tokenize input with a streaming lexer.
- Parse it into an AST with a top-level structure.
- Evaluate expressions with a small set of built-in forms (`begin`, `define`, `print`, `quote`, basic arithmetic).

#### Functions and better errors

> 14 December 2025

- feat: Enabled lambda functions for storing reusable code.
- feat: Switched from immediate process exit on errors to returning error values from evaluation.
- feat: Introduced `AST_ERROR` nodes to represent errors explicitly.
- refactor: Moved most builtins into a dedicated `builtins/` folder.
- docs: Expanded `README.md` with the new semantics.

From the user’s point of view this enabled:

- Defining and using simple functions.
- Seeing evaluation and parsing errors as values instead of hard exits.

#### Architecture cleanups

> 9 January 2026

- refactor: Separated builtin forms from the evaluator core.
- refactor: Split AST definitions from parser code.
- refactor: Removed the `odeus_` prefix from many identifiers.
- refactor: Removed the unnecessary top-level CONS cell around programs.
- build: Separated and cleaned up `CMakeLists.txt` files for core and builtins.

#### Core language grows up

> 12–15 January 2026

- refactor: Moved core headers to `core/include/core` and builtin headers to `builtins/include/builtins`.
- feat: Added control-flow builtins `if`, `and`, and `or`.
- feat: Added list builtins for working with lists and pairs.
- feat: Added math and numeric comparison operators.
- feat: Added type predicates and consolidated around the `typeof` primitive.
- feat: Introduced string manipulation functions.
- fix: Resolved nested quasiquote bugs and added a `make_quote` helper.

During this period Odeus became a more complete Lisp with:

- Real control flow.
- Lists and other basic data structures.
- Numbers, comparisons, and strings.

#### Std I/O, forms, and performance

> 17 January 2026

- feat: Added stdio builtins and additional core forms as builtins.
- perf: Implemented symbol interning for faster symbol lookup.
- feat: Renamed the entry point from `repl.c` to `odeus.c`.
- feat: Allowed the binary to either run a file or start a REPL depending on arguments.
- chore: Added `.gitattributes` and performed code formatting passes.
- docs: Added the initial `CHANGELOG.md` and `DOCS.md`.

#### Macros, quasiquote, and documentation

> 20–26 January 2026

- feat: Added syntax sugar for quasiquote (backtick and unquote).
- feat: Implemented anonymous macros.
- feat: Added the `load-file` builtin and refined lexer behavior.
- feat: Added shorthand for defining functions: `(define (name args...) body...)`.
- fix: Corrected rest-parameter handling for lambdas.
- refactor: Removed `AST_QUOTE` as a separate AST type.
- refactor: Moved macro and quasiquote helpers into their own files.
- feat: Added environments to macro values so macros see their definition environment.
- feat: Introduced `macroexpand` and improved printing of lambda and macro nodes.
- feat: Started implementing some builtins directly in Odeus (in the prelude) instead of C.

This was a major leap: Odeus gained a real macro system with quasiquote, bringing it closer to a Scheme-style Lisp.

#### Values, apply, and environments

> 28–30 January 2026

- feat: Added builtins such as `apply` and more helpers.
- refactor: Consolidated AST builtin node kinds into a single `AST_BUILTIN` type.
- refactor: Removed the distinction between “special” and “normal” builtins.
- refactor: Renamed `AST` to `Val` and then to `Value`, and `Env` to `Environment`.
- refactor: Updated the parser to produce AST/Value structures that better match the evaluator.
- feat: Introduced a dedicated `Environment` struct, simplifying scope and name lookup.

#### Meta information and GC

> 1–8 February 2026

- feat: Added meta information to symbols (e.g. filename and line number).
- feat: Added `show-meta` to inspect where a binding was defined.
- feat: Improved `val_error` with variadic arguments for richer error messages.
- feat: Added `reload-file` to support reloading files in the REPL.
- feat: Integrated a Boehm–Weiser–Hudson (BWH) garbage collector.
- feat: Added a `symbol_map` for interning symbols and improving performance.
- feat: Moved higher-level helpers (like `filter`) into `prelude.ode`.

#### Closure model and modules

> 11–22 February 2026

- refactor: Unified `LAMBDA` and `MACRO` into a single closure representation using an anonymous union.
- feat: Added the first version of a module system:
  - Introduced a `module` value type.
  - Allowed importing modules and using them as first-class values.

This made Odeus more modular: code could be split into files and loaded as modules.

#### Module cleanup and gensym

> 4–7 March 2026

- refactor: Removed the global module registry so modules are purely first-class values without hidden global state.
- feat: Updated `typeof` to recognize the `module` value type.
- feat: Added `gensym` and updated prelude macros to use it, preventing accidental variable capture.
- docs: Updated `DOCS.md` to reflect the current language behavior and module semantics.
