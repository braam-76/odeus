# Odeus language documentation

Odeus is a Lisp-style language implemented in C. This document describes the language as implemented: syntax, built-in forms, and how to use them.

---

## 1. Running Odeus

**REPL** (no arguments):

```sh
./odeus
```

**Run a file**:

```sh
./odeus <filename>
```

Example: `./odeus script.ode`

**Build** (from repo root):

```sh
mkdir build && cd build
cmake ../
make
```

---

## 2. Syntax overview

- **Lists**: `(a b c)` — parentheses, space-separated elements.
- **Dotted pair**: `(a . b)` — one cons cell with CAR `a` and CDR `b`.
- **Literals**: integers (`42`), floats (`3.14`), strings (`"hello"`), symbols (`foo`, `+`, `my-var`).
- **Quote**: `'x` or `(quote x)` — return the expression without evaluating.
- **Quasiquote**: `` `expr `` — template; `,expr` evaluates and inserts a value; `,@expr` splices a list.
- **Comments**: one-line comments starting with `;` symbol.

Anything not in double quotes is tokenized as symbol or number; parentheses, quote, backtick, comma, and `,@` are special.

---

## 3. Value types

| Type       | Description                          |
|-----------|--------------------------------------|
| `nil`     | Empty list / false.                  |
| `t`       | Canonical true value.               |
| symbol    | Identifier (interned).              |
| integer   | Whole numbers.                      |
| float     | Floating-point.                     |
| string    | `"..."`                             |
| cons      | Pair (CAR . CDR), used for lists.   |
| function  | Lambda or builtin.                  |
| macro     | Macro (transforms syntax).          |
| module    | Result of `import`.                  |
| error     | Error value (message string).       |

Predicates: use `(typeof x)` and compare to symbols `'nil`, `'symbol`, `'integer`, `'float`, `'string`, `'cons`, `'function`, `'macro`, `'module`, `'error`.

---

## 4. Core forms and special operators

### 4.1 Definitions and mutation

- **`(define symbol expr)`**  
  Defines a variable. Evaluates `expr` and binds it to `symbol`. Error if `symbol` is already defined.

- **`(define (name arg ...) body ...)`**  
  Shorthand for defining a function:  
  `(define name (lambda (arg ...) body ...))`.

- **`(set! symbol expr)`**  
  Mutates an existing binding. Evaluates `expr` and assigns to `symbol`. Error if `symbol` is not defined.

### 4.2 Binding forms

- **`(let ((sym val) ...) body ...)`**  
  Local bindings. All `val` expressions are evaluated in the environment *outside* the `let`; then all `body` forms are evaluated in an environment where each `sym` is bound to the corresponding result. Returns the value of the last `body` form.

- **`(let* ((sym val) ...) body ...)`**  
  Sequential bindings. Each `val` is evaluated in an environment that already contains the previous bindings. Then `body ...` is evaluated in the full binding environment.

### 4.3 Control flow

- **`(if cond then else)`**  
  Evaluates `cond`; if non-nil, evaluates and returns `then`, otherwise evaluates and returns `else`.

- **`(begin expr ...)`**  
  Evaluates expressions in order; returns the value of the last one.

- **`(eq a b)`**  
  Identity comparison: returns `t` only if `a` and `b` are the same object (for nils, symbols, builtins). Not for structural equality of lists or numbers.

- **`(and expr ...)`**  
  Short-circuit and: evaluates left to right; returns the first nil, or the last value if all are non-nil.

- **`(or expr ...)`**  
  Short-circuit or: returns the first non-nil value, or nil if all are nil.

### 4.4 Quote and quasiquote

- **`(quote x)`** / **`'x`**  
  Returns `x` without evaluation.

- **`(quasiquote expr)`** / **`` `expr ``**  
  Builds a structure from `expr`, evaluating only where unquote (`,expr`) and unquote-splicing (`,@expr`) appear.  
  Splicing uses `append`; if you use `,@` you need `append` in scope (e.g. from a prelude or your own definition).

---

## 5. Functions and application

- **`(lambda (arg ...) body ...)`**  
  Creates a closure: parameters and body in a list, captured current environment. Rest parameter: a single symbol in the parameter list receives the rest of the arguments as a list.

- **`(apply proc list)`**  
  Applies `proc` to the elements of `list` as arguments. Expects two arguments: a function value and a list value.

- **`(eval expr)`**  
  Evaluates `expr` once to get a value, then evaluates that value again in the current environment. So `(eval x)` is “evaluate x, then evaluate the result.”

---

## 6. Lists and pairs

| Form            | Description |
|-----------------|-------------|
| `(cons a b)`    | Builds a pair (CAR = `a`, CDR = `b`). |
| `(list x ...)`  | Builds a proper list of the evaluated arguments. |
| `(car pair)`    | First element of a pair. |
| `(cdr pair)`    | Second element of a pair. |
| `(set-car! pair val)` | Mutates CAR of `pair`. |
| `(set-cdr! pair val)` | Mutates CDR of `pair`. |
| `(length list)`  | Number of elements in a proper list. |
| `(reverse list)` | New list with elements reversed. |

---

## 7. Numeric operations

All accept integers and floats; result is integer only when all operands are integers and the operation is exact.

| Operator | Form           | Description |
|----------|----------------|-------------|
| `+`      | `(+ n ...)`    | Sum (zero args → 0). |
| `-`      | `(- n ...)`    | Negation; `(- a b ...)` subtracts rest from first. |
| `*`      | `(* n ...)`    | Product (zero args → 1). |
| `/`      | `(/ a b ...)`  | Division; always returns float. |
| `mod`    | `(mod a b)`    | Remainder (integer). |
| `expt`   | `(expt base exp)` | Power. |
| `sqrt`   | `(sqrt x)`     | Square root (float). |
| `abs`    | `(abs x)`      | Absolute value. |
| `floor`  | `(floor x)`    | Floor (integer). |
| `ceil`   | `(ceil x)`     | Ceiling (integer). |
| `round`  | `(round x)`    | Round (integer). |

Comparisons (return `t` or `nil`): `=`, `>`, `<`, `>=`, `<=`.

---

## 8. Strings

| Form | Description |
|------|-------------|
| `(concat s ...)` | Concatenates strings. |
| `(string-length s)` | Length of string. |
| `(substring s start [end])` | Substring; negative indices from end; default end is length. |
| `(symbol->string sym)` | Name of symbol as string. |
| `(string->symbol s)` | Not implemented (returns error). |

---

## 9. Type and I/O

- **`(typeof x)`**  
  Returns a symbol describing the type: `nil`, `symbol`, `integer`, `float`, `string`, `cons`, `function`, `macro`, `module`, or `error`.

- **`(dump x ...)`**  
  Prints values with `value_print` (readable representation), space-separated, then newline. Returns nil.

- **`(write x)`**  
  Returns a string representation of `x` (as from `value_to_string`).

- **`(display x)`**  
  Prints a simplified representation of `x` (no quotes around strings, etc.). Returns nil.

- **`(read string)`**  
  Parses the string as Odeus code and returns the parsed value (one expression). Does not evaluate.

- **`(read-file path)`**  
  Reads file at `path` (string), parses as Odeus, returns parsed value. Does not evaluate.

- **`(load-file path)`**  
  Reads and evaluates the file in the current environment. Returns the last result or errors.

- **`(reload-file path)`**  
  Unbinds bindings that were defined from the given filename, then loads the file again.

- **`(file->string path)`**  
  Returns file contents as a single string.

- **`(show-meta symbol)`**  
  Prints filename and line number of the binding of `symbol` (for debugging). Returns nil.

---

## 10. Modules

Modules are first-class values; there is no global module registry. You bind a module to a variable and use it from there.

- **`(import path)`**  
  `path` must be a **string** (e.g. `"mymod.ode"`). Loads and evaluates the file in a fresh environment (child of the current one) and returns a **module value**. Does not register the module anywhere; you bind the result yourself.

  Example:

  ```scheme
  (define mymod (import "mymod.ode"))
  ```

  Symbol argument (e.g. `(import fs)` for a future standard module) is not supported yet and signals an error.

- **`(get-from-module module symbol)`**  
  `module` must be a module value (e.g. the result of `import`). Looks up `symbol` in that module’s environment and returns its value. Error if not a module or symbol is unbound.

- **Module/symbol syntax**  
  The token `module/symbol` is rewritten at parse time to `(get-from-module module symbol)`. So if `mymod` is bound to a module that has `foo`, you can write:

  ```scheme
  mymod/foo
  ```

  and it behaves like `(get-from-module mymod foo)`.

---

## 11. Macros

- **`(macro (param ...) body ...)`**  
  Creates a macro: receives unevaluated argument trees, runs in an environment whose parent is the definition environment, returns one value that is then evaluated. Parameters are bound to the raw argument forms; rest parameter (single symbol) gets the rest as a list.

- **`(defmacro (name param ...) body ...)`**  
  Defines a macro in the current environment. Same as defining a variable whose value is `(macro (param ...) body ...)`. The macro name must not already be defined.

- **`(macroexpand expr)`**  
  Expands macros in `expr` (one level) and returns the result. Does not evaluate further. Useful for debugging macros.

### 11.1 gensym — fresh symbols for macros

Because macros are unhygienic, any identifier you introduce in the expansion (e.g. a temporary variable) can clash with the user’s bindings: the user might have a variable with the same name, or your temp might accidentally capture the user’s identifier. To avoid that, use **gensym** to generate a new symbol that is guaranteed not to exist elsewhere.

- **`(gensym)`**  
  Returns a **new symbol** with a unique name each time (e.g. `g__0`, `g__1`, `g__2`, …). Takes no arguments. Each call returns a different symbol.

- **`(gensym prefix)`**  
  Returns a new symbol whose **name starts with** `prefix`. The only argument is a **symbol**; its print name is used as the prefix (e.g. `(gensym tmp)` → `tmp0`, `tmp1`, …). Useful when debugging macro expansions so you can recognise your temps (e.g. `tmp`, `result`, `loop`).

**When to use:** Whenever your macro inserts a **binding** (e.g. a `let` variable, a lambda parameter) that is only for internal use — create it with `gensym` so it never clashes with user code.

**Example** — a `when` macro that must not capture a user’s `tmp`:

```scheme
(defmacro (when cond . body)
  (let ((tmp (gensym)))
    `(let ((,tmp ,cond))
       (if ,tmp (begin ,@body) nil))))
```

Here `tmp` is bound to a fresh symbol (e.g. `g__42`) inside the macro, and the expansion uses that symbol in `(let ((g__42 <cond>)) ...)`. So user code can use a variable named `tmp` and it will not be affected.

Macros are unhygienic by default: identifiers you introduce are normal symbols. Use `gensym` (or a helper like `with-gensyms`) for all temporary bindings in macro output.

---

## 12. Summary of built-in names

**Core / control:** `begin`, `eval`, `if`, `eq`, `and`, `or`  
**Variables:** `define`, `set!`, `let`, `let*`  
**Quote:** `quote`, `quasiquote`  
**Functions:** `lambda`, `apply`  
**Lists:** `cons`, `list`, `car`, `cdr`, `set-car!`, `set-cdr!`, `length`, `reverse`  
**Numbers:** `+`, `-`, `*`, `/`, `mod`, `expt`, `sqrt`, `abs`, `floor`, `ceil`, `round`, `=`, `>`, `<`, `>=`, `<=`  
**Type:** `typeof`  
**Strings:** `concat`, `string-length`, `substring`, `string->symbol`, `symbol->string`  
**I/O:** `dump`, `read`, `read-file`, `load-file`, `reload-file`, `file->string`, `write`, `display`, `show-meta`  
**Modules:** `import`, `get-from-module`  
**Macros:** `macro`, `defmacro`, `macroexpand`, `gensym`  
**Constants:** `t`, `nil`

---

## 13. Files and structure (reference)

- **Entry:** `bin/odeus.c` — REPL or file execution, single global environment.
- **Evaluation:** `core/eval.c` — `evaluate_expression`, `apply`, `macro_expand_expression`; macros expanded before application.
- **Values:** `core/value.c`, `core/include/core/value.h` — value types and constructors; `val_module` is a pure constructor (no registry).
- **Environments:** `core/environment.c` — parent-linked frames; bindings by symbol identity.
- **Parser:** `core/parser.c` — lists, dotted pairs, quote, quasiquote, unquote, `module/symbol` → `(get-from-module module symbol)`.
- **Builtins:** `builtins/set_builtins.c` registers all builtins; individual files under `builtins/` (e.g. `forms.c`, `list.c`, `module.c`, `macros.c`).
- **Quasiquote:** `core/quasiquote.c` — expands to `quote`, `cons`, and `append`; `,@` requires `append` in scope.

This document reflects the current implementation; for the latest behavior, refer to the source.
