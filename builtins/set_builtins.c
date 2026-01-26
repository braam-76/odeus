#include "builtins/set_builtins.h"

#include "builtins/constrol_flow.h"
#include "builtins/forms.h"
#include "builtins/list.h"
#include "builtins/macros.h"
#include "builtins/math.h"
#include "builtins/stdio.h"
#include "builtins/strings.h"
#include "builtins/typep.h"

#define NORMAL(name, fn)                                                      \
  environment_set (environment, make_symbol (name),                           \
                   make_builtin (fn, AST_BUILTIN_NORMAL))

#define SPECIAL(name, fn)                                                     \
  environment_set (environment, make_symbol (name),                           \
                   make_builtin (fn, AST_BUILTIN_SPECIAL))

void
set_builtins (AST *environment)
{
  environment_set (environment, make_symbol ("t"), t ());
  environment_set (environment, make_symbol ("nil"), nil ());

  SPECIAL ("begin", builtin_begin);
  NORMAL ("eval", builtin_eval);

  // Control flow
  SPECIAL ("if", builtin_if);
  NORMAL ("eq", builtin_eq);
  NORMAL ("null", builtin_null); // lisper's not operator
  NORMAL ("not", builtin_null);  // human readable not operator
  NORMAL ("and", builtin_and);
  NORMAL ("or", builtin_or);

  // List operations
  NORMAL ("cons", builtin_cons);
  NORMAL ("list", builtin_list);
  NORMAL ("append", builtin_append);
  NORMAL ("car", builtin_car);
  NORMAL ("cdr", builtin_cdr);
  NORMAL ("set-car!", builtin_set_car);
  NORMAL ("set-cdr!", builtin_set_cdr);
  NORMAL ("length", builtin_length);
  NORMAL ("reverse", builtin_reverse);
  NORMAL ("map", builtin_map);
  NORMAL ("filter", builtin_filter);

  // Comparison operators
  NORMAL ("=", builtin_num_eq);
  NORMAL (">", builtin_num_gt);
  NORMAL ("<", builtin_num_lt);
  NORMAL (">=", builtin_num_gte);
  NORMAL ("<=", builtin_num_lte);

  // Type predicates
  NORMAL ("atom?", builtin_is_atom);
  NORMAL ("symbol?", builtin_is_symbol);
  NORMAL ("string?", builtin_is_string);
  NORMAL ("number?", builtin_is_number);
  NORMAL ("integer?", builtin_is_integer);
  NORMAL ("float?", builtin_is_float);
  NORMAL ("cons?", builtin_is_cons);
  NORMAL ("function?", builtin_is_function);

  // String operations
  NORMAL ("concat", builtin_concat);
  NORMAL ("string-length", builtin_string_length);
  NORMAL ("substring", builtin_substring);
  NORMAL ("string->symbol", builtin_string_to_symbol);
  NORMAL ("symbol->string", builtin_symbol_to_string);

  // I/O operations
  NORMAL ("dump", builtin_dump);
  NORMAL ("read", builtin_read);
  NORMAL ("read-file", builtin_read_file); // expects lisp code
  NORMAL ("load-file", builtin_load_file);
  NORMAL ("file->string", builtin_file_to_string); // just reads file as string
  NORMAL ("write", builtin_write);
  NORMAL ("display", builtin_display);

  // Math functions
  NORMAL ("+", builtin_add);
  NORMAL ("-", builtin_sub);
  NORMAL ("*", builtin_mul);
  NORMAL ("/", builtin_div);
  NORMAL ("mod", builtin_mod);
  NORMAL ("expt", builtin_expt);
  NORMAL ("sqrt", builtin_sqrt);
  NORMAL ("abs", builtin_abs);
  NORMAL ("floor", builtin_floor);
  NORMAL ("ceil", builtin_ceil);
  NORMAL ("round", builtin_round);

  // Variable managment
  SPECIAL ("define", builtin_define);
  SPECIAL ("set!", builtin_set);
  SPECIAL ("let", builtin_let);
  SPECIAL ("let*", builtin_let_star);

  SPECIAL ("quote", builtin_quote);
  SPECIAL ("lambda", builtin_lambda);
  SPECIAL ("macro", builtin_macro);
  SPECIAL ("defmacro", builtin_defmacro);
  SPECIAL ("quasiquote", builtin_quasiquote);
}
