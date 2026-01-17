# Odeus documentation

## Primitives

Odeus programming language has 5 atomic types:

- Nil
- Integer
- Float
- String
- Symbol

Odeus does not have native booleans. 
- nil represents false.
- t represents true.
- Any other value is considered truthy in conditionals.

## Complex constructs

Odeus programming language has only 1 complex type: *cons* which is pair of values (*car* . *cdr*),
*car* stores pointer to current element (basically, just value, can be either atom or another cons).
*cdr* usually stores pointer to the next cons cell (but if it is improper list, it can store atom).
To understand, those 2 lists are equal in this language

```scheme
(list 1 2 3 4) ;; == (cons 1 (cons 2 (cons 3 (cons 4 nil))))
```

and in fact, builtin function *list* produces that pair of pairs. This builtin is just easier and faster way to write same thing on the right of *==*

## Quote

Quote in this programming language is type, which represents not yet evaluated expression. It can be atom, cons, function call, everything. 

```scheme
(display "some string") ;; prints "some string"
'(display "some string") ;; returns itself: '(display "some string")
(quote (display "some string")) ;; same thing as above with quote symbol (')
```

and quote can be used as the way to represent list (just like *list* function), meaning

```scheme
(car '(display "some string")) ; retuns 'display' symbol
(cdr '(display "some string")) ; retuns ("some string") 
```

## Function call

Function call created by putting symbol as first element in list. Then evaluator searches for that symbol in environment, and if it finds symbol binding, then it executes function assigned to symbol.

```scheme
;; Example
(car (list 1 2 3 4)) ;; => 1
(cdr (list 1 2 3 4)) ;; => (2 3 4)
```

## Special forms

Special forms control evaluation; their arguments may **not be evaluated automatically**.

| Form         | Syntax / Description                                                                                               | Example                                            |
| ------------ | ------------------------------------------------------------------------------------------------------------------ | -------------------------------------------------- |
| `quote`      | `(quote expr)` returns `expr` without evaluating it                                                                | `(quote x)` → `x`                                  |
| `if`         | `(if condition then else)` evaluates `condition`; returns `then` if true, `else` otherwise                         | `(if (< 1 2) 'yes 'no)` → `yes`                    |
| `define`     | `(define symbol expr)` defines a new variable with the evaluated value of `expr`. Cannot redefine existing symbols | `(define x 10)`                                    |
| `set!`       | `(set! symbol expr)` updates an existing variable                                                                  | `(set! x 20)`                                      |
| `lambda`     | `(lambda (params ...) body ...)` creates an anonymous function                                                     | `((lambda (x) (+ x 1)) 5)` → `6`                   |
| `let`        | `(let ((var val) ...) body ...)` creates a new environment and evaluates body                                      | `(let ((x 2) (y 3)) (+ x y))` → `5`                |
| `let*`       | `(let* ((var val) ...) body ...)` like `let` but sequential bindings (supports recursion)                          | `(let* ((x 2) (y x)) (+ x y))` → `4`               |
| `quasiquote` | `(quasiquote expr)` allows unquoting and splicing inside `expr`                                                    | `(quasiquote (1 2 (unquote (+ 1 1))))` → `(1 2 2)` |

---

## List Primitives

| Function  | Description                                                    | Example                                              |
| --------- | -------------------------------------------------------------- | ---------------------------------------------------- |
| `cons`    | `(cons a b)` creates a pair                                    | `(cons 1 2)` → `(1 . 2)`                             |
| `car`     | `(car pair)` returns the first element of a pair               | `(car (cons 1 2))` → `1`                             |
| `cdr`     | `(cdr pair)` returns the second element of a pair              | `(cdr (cons 1 2))` → `2`                             |
| `list`    | `(list a b c ...)` creates a proper list                       | `(list 1 2 3)` → `(1 2 3)`                           |
| `append`  | `(append list1 list2 ...)` concatenates lists                  | `(append '(1 2) '(3 4))` → `(1 2 3 4)`               |
| `length`  | `(length lst)` returns the length of a list                    | `(length '(a b c))` → `3`                            |
| `reverse` | `(reverse lst)` returns a reversed copy of the list            | `(reverse '(1 2 3))` → `(3 2 1)`                     |
| `map`     | `(map func lst)` applies `func` to each element                | `(map (lambda (x) (* x 2)) '(1 2 3))` → `(2 4 6)`    |
| `filter`  | `(filter pred lst)` returns elements where `pred` returns true | `(filter (lambda (x) (> x 2)) '(1 2 3 4))` → `(3 4)` |

---

## Comparison Operators

| Operator | Description           | Example          |
| -------- | --------------------- | ---------------- |
| `=`      | Numeric equality      | `(= 3 3)` → `t`  |
| `<`      | Less than             | `(< 2 3)` → `t`  |
| `>`      | Greater than          | `(> 5 2)` → `t`  |
| `<=`     | Less than or equal    | `(<= 3 3)` → `t` |
| `>=`     | Greater than or equal | `(>= 5 2)` → `t` |

---

## Arithmetic

| Function | Description              | Example             |
| -------- | ------------------------ | ------------------- |
| `+`      | Addition                 | `(+ 1 2 3)` → `6`   |
| `-`      | Subtraction              | `(- 5 2)` → `3`     |
| `*`      | Multiplication           | `(* 2 3)` → `6`     |
| `/`      | Division                 | `(/ 6 2)` → `3`     |
| `mod`    | Modulus                  | `(mod 5 2)` → `1`   |
| `expt`   | Power                    | `(expt 2 3)` → `8`  |
| `abs`    | Absolute value           | `(abs -5)` → `5`    |
| `floor`  | Floor function           | `(floor 3.7)` → `3` |
| `ceil`   | Ceiling function         | `(ceil 3.2)` → `4`  |
| `round`  | Round to nearest integer | `(round 3.6)` → `4` |
| `sqrt`   | Square root              | `(sqrt 16)` → `4`   |

---

## Type Predicates

| Predicate   | Description                   | Example                            |
| ----------- | ----------------------------- | ---------------------------------- |
| `atom?`     | Checks if value is not a pair | `(atom? 1)` → `t`                  |
| `symbol?`   | Checks if value is a symbol   | `(symbol? 'x)` → `t`               |
| `string?`   | Checks if value is a string   | `(string? "hi")` → `t`             |
| `number?`   | Checks if value is a number   | `(number? 3.14)` → `t`             |
| `integer?`  | Checks if value is integer    | `(integer? 5)` → `t`               |
| `float?`    | Checks if value is float      | `(float? 5.0)` → `t`               |
| `cons?`     | Checks if value is a pair     | `(cons? '(1 2))` → `t`             |
| `function?` | Checks if value is a function | `(function? (lambda (x) x))` → `t` |

---

## I/O Functions

| Function       | Description                             | Example                              |
| -------------- | --------------------------------------- | ------------------------------------ |
| `display`      | Prints value to stdout                  | `(display "hello")` → prints `hello` |
| `dump`         | Prints internal AST representation      | `(dump '(1 2 3))` → `(1 2 3)`        |
| `read`         | Reads a Lisp expression from input      | `(read)` → user input expression     |
| `read-file`    | Reads a file and returns content as AST | `(read-file "file.odeus")`           |
| `write`        | Turns expression into string | `(write "file.odeus" '(1 2 3))`      |
| `file->string` | Reads file content as string            | `(file->string "file.txt")`          |

## Example Programs

### Factorial

```lisp
(define factorial
  (lambda (n)
    (if (= n 0)
        1
        (* n (factorial (- n 1))))))
(factorial 5) ; => 120
```

