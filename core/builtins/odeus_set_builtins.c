#include "odeus_set_builtins.h"

#include "builtins/odeus_forms.h"
#include "builtins/odeus_primitives.h"
#include "builtins/odeus_basic_math.h"

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

  REGISTER_SPECIAL ("quote", builtin_quote);
  REGISTER_NORMAL ("atom", builtin_atom);
  REGISTER_NORMAL ("eq", builtin_eq);
  REGISTER_NORMAL ("cons", builtin_cons);
  REGISTER_NORMAL ("null", builtin_null); // lisper's not operator
  REGISTER_NORMAL ("not", builtin_null);  // human readable not operator
  REGISTER_SPECIAL ("lambda", builtin_lambda);

  REGISTER_SPECIAL ("define", builtin_define);
  REGISTER_NORMAL ("print", builtin_print);
  REGISTER_NORMAL ("+", builtin_add);
  REGISTER_NORMAL ("-", builtin_sub);
  REGISTER_NORMAL ("*", builtin_mul);
  REGISTER_NORMAL ("/", builtin_div);
}
