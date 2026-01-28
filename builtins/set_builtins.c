#include "builtins/set_builtins.h"

#include "builtins/constrol_flow.h"
#include "builtins/forms.h"
#include "builtins/list.h"
#include "builtins/macros.h"
#include "builtins/math.h"
#include "builtins/stdio.h"
#include "builtins/strings.h"
#include "builtins/typeof.h"

#define REGISTER(name, fn)                                                    \
  environment_set (environment, make_symbol (name), make_builtin (fn))

void
set_builtins (AST *environment)
{
  environment_set (environment, make_symbol ("t"), t ());
  environment_set (environment, make_symbol ("nil"), nil ());

  REGISTER ("begin", builtin_begin);
  REGISTER ("eval", builtin_eval);

  // Control flow
  REGISTER ("if", builtin_if);
  REGISTER ("eq", builtin_eq);
  REGISTER ("and", builtin_and);
  REGISTER ("or", builtin_or);

  // List operations
  REGISTER ("cons", builtin_cons);
  REGISTER ("list", builtin_list);
  REGISTER ("car", builtin_car);
  REGISTER ("cdr", builtin_cdr);
  REGISTER ("set-car!", builtin_set_car);
  REGISTER ("set-cdr!", builtin_set_cdr);
  REGISTER ("length", builtin_length);
  REGISTER ("reverse", builtin_reverse);
  REGISTER ("filter", builtin_filter);
  REGISTER ("apply", builtin_apply);

  // Comparison operators
  REGISTER ("=", builtin_num_eq);
  REGISTER (">", builtin_num_gt);
  REGISTER ("<", builtin_num_lt);
  REGISTER (">=", builtin_num_gte);
  REGISTER ("<=", builtin_num_lte);

  REGISTER ("typeof", builtin_typeof);

  // String operations
  REGISTER ("concat", builtin_concat);
  REGISTER ("string-length", builtin_string_length);
  REGISTER ("substring", builtin_substring);
  REGISTER ("string->symbol", builtin_string_to_symbol);
  REGISTER ("symbol->string", builtin_symbol_to_string);

  // I/O operations
  REGISTER ("dump", builtin_dump);
  REGISTER ("read", builtin_read);
  REGISTER ("read-file", builtin_read_file); // expects lisp code
  REGISTER ("load-file", builtin_load_file);
  REGISTER ("file->string",
            builtin_file_to_string); // just reads file as string
  REGISTER ("write", builtin_write);
  REGISTER ("display", builtin_display);

  // Math functions
  REGISTER ("+", builtin_add);
  REGISTER ("-", builtin_sub);
  REGISTER ("*", builtin_mul);
  REGISTER ("/", builtin_div);
  REGISTER ("mod", builtin_mod);
  REGISTER ("expt", builtin_expt);
  REGISTER ("sqrt", builtin_sqrt);
  REGISTER ("abs", builtin_abs);
  REGISTER ("floor", builtin_floor);
  REGISTER ("ceil", builtin_ceil);
  REGISTER ("round", builtin_round);

  // Variable managment
  REGISTER ("define", builtin_define);
  REGISTER ("set!", builtin_set);
  REGISTER ("let", builtin_let);
  REGISTER ("let*", builtin_let_star);

  REGISTER ("quote", builtin_quote);
  REGISTER ("lambda", builtin_lambda);
  REGISTER ("macro", builtin_macro);
  REGISTER ("defmacro", builtin_defmacro);
  REGISTER ("macroexpand", builtin_macroexpand);
  REGISTER ("quasiquote", builtin_quasiquote);
}
