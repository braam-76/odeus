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
  REGISTER_NORMAL("eval", builtin_eval);

  // Control flow
  REGISTER_NORMAL ("eq", builtin_eq);
  REGISTER_NORMAL ("null", builtin_null); // lisper's not operator
  REGISTER_NORMAL ("not", builtin_null);  // human readable not operator
  REGISTER_NORMAL("if", builtin_if);
  REGISTER_NORMAL("and", builtin_and);
  REGISTER_NORMAL("or", builtin_or);

  // List operations
  REGISTER_NORMAL ("cons", builtin_cons);
  REGISTER_NORMAL ("list", builtin_list);
  REGISTER_NORMAL ("append", builtin_append);
  // car
  // cdr
  // set-car!
  // set-cdr!
  // length
  // reverse
  // map
  // filder
  // foldr

  // Comparison operators
  // = (numeric equality)
  // >
  // <
  // >=
  // <=

  // Type predicates
  REGISTER_NORMAL ("atom?", builtin_is_atom);
  // symbol?
  // string?
  // number?
  // integer?
  // float?
  // list?
  // cons?
  // function?

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
  // %
  // mod
  // expt
  // sqrt
  // abs
  // floor
  // ceil
  // round

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
