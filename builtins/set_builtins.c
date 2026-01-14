#include "builtins/set_builtins.h"

#include "builtins/constrol_flow.h"
#include "builtins/forms.h"
#include "builtins/list.h"
#include "builtins/math.h"
#include "builtins/stdio.h"
#include "builtins/strings.h"
#include "builtins/typep.h"

#define REGISTER_NORMAL(name, fn)                                                                  \
  environment_set (environment, make_symbol (name), make_builtin (fn, AST_BUILTIN_NORMAL))

#define REGISTER_SPECIAL(name, fn)                                                                 \
  environment_set (environment, make_symbol (name), make_builtin (fn, AST_BUILTIN_SPECIAL))

void
set_builtins (AST *environment)
{
  environment_set (environment, make_symbol ("t"), t ());
  environment_set (environment, make_symbol ("nil"), nil ());

  REGISTER_SPECIAL ("begin", builtin_begin);
  REGISTER_NORMAL ("eval", builtin_eval);

  // Control flow
  REGISTER_NORMAL ("eq", builtin_eq);
  REGISTER_NORMAL ("null", builtin_null); // lisper's not operator
  REGISTER_NORMAL ("not", builtin_null);  // human readable not operator
  REGISTER_NORMAL ("if", builtin_if);
  REGISTER_NORMAL ("and", builtin_and);
  REGISTER_NORMAL ("or", builtin_or);

  // List operations
  REGISTER_NORMAL ("cons", builtin_cons);
  REGISTER_NORMAL ("list", builtin_list);
  REGISTER_NORMAL ("append", builtin_append);
  REGISTER_NORMAL ("car", builtin_car);
  REGISTER_NORMAL ("cdr", builtin_cdr);
  REGISTER_NORMAL ("set-car!", builtin_set_car);
  REGISTER_NORMAL ("set-cdr!", builtin_set_cdr);
  REGISTER_NORMAL ("length", builtin_length);
  REGISTER_NORMAL ("reverse", builtin_reverse);
  REGISTER_NORMAL ("map", builtin_map);
  REGISTER_NORMAL ("filter", builtin_filter);

  // Comparison operators
  REGISTER_NORMAL ("=", builtin_num_eq);
  REGISTER_NORMAL (">", builtin_num_gt);
  REGISTER_NORMAL ("<", builtin_num_lt);
  REGISTER_NORMAL (">=", builtin_num_gte);
  REGISTER_NORMAL ("<=", builtin_num_lte);

  // Type predicates
  REGISTER_NORMAL ("atom?", builtin_is_atom);
  REGISTER_NORMAL ("symbol?", builtin_is_symbol);
  REGISTER_NORMAL ("string?", builtin_is_string);
  REGISTER_NORMAL ("number?", builtin_is_number);
  REGISTER_NORMAL ("integer?", builtin_is_integer);
  REGISTER_NORMAL ("float?", builtin_is_float);
  REGISTER_NORMAL ("cons?", builtin_is_cons);
  REGISTER_NORMAL ("function?", builtin_is_function);

  // String operations
  // concat
  // string-length
  // substring
  // string->symbol
  // symbol->string

  // I/O operations
  // REGISTER_NORMAL ("dump", builtin_dump);
  // read
  // read-file
  // write
  // display
  // newline

  // Math functions
  REGISTER_NORMAL ("+", builtin_add);
  REGISTER_NORMAL ("-", builtin_sub);
  REGISTER_NORMAL ("*", builtin_mul);
  REGISTER_NORMAL ("/", builtin_div);
  REGISTER_NORMAL ("mod", builtin_mod);
  REGISTER_NORMAL ("expt", builtin_expt);
  REGISTER_NORMAL ("sqrt", builtin_sqrt);
  REGISTER_NORMAL ("abs", builtin_abs);
  REGISTER_NORMAL ("floor", builtin_floor);
  REGISTER_NORMAL ("ceil", builtin_ceil);
  REGISTER_NORMAL ("round", builtin_round);

  // Variable managment
  REGISTER_SPECIAL ("define", builtin_define);
  // set!
  // let
  // let*
  // letrec

  REGISTER_SPECIAL ("quote", builtin_quote);
  REGISTER_SPECIAL ("lambda", builtin_lambda);
  REGISTER_SPECIAL ("quasiquote", builtin_quasiquote);
}
