#include "builtins/macros.h"

AST *
builtin_macro (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("macro: expects parameter list");

  AST *parameters = CAR (arguments);
  AST *body = CDR (arguments);

  AST *macro = malloc (sizeof (AST));
  macro->type = AST_MACRO;
  macro->as.MACRO.parameters = parameters;
  macro->as.MACRO.body = body;
  macro->as.MACRO.environment = environment;

  return macro;
}

AST *
builtin_defmacro (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return make_error (
        "defmacro: expects (macro-name args) expr");

  AST *to_be_defined = CAR (arguments);

  if (to_be_defined->type == AST_CONS)

    if (IS_NULL (to_be_defined))
      return make_error ("defmacro: macro definition cannot be empty");

  AST *func_name = CAR (to_be_defined);
  if (func_name->type != AST_SYMBOL)
    return make_error ("defmacro: macro name must be a symbol");

  AST *current = environment_get (environment, func_name);
  if (current->type != AST_ERROR)
    return make_error ("defmacro: symbol already defined");

  AST *params = CDR (to_be_defined);

  // Initialize with nil first
  environment_set (environment, func_name, nil ());

  AST *macro_args = make_cons (params, CDR (arguments));
  AST *macro = builtin_macro (environment, macro_args);
  ERROR_OUT (macro);

  // Update the environment with the macro
  environment_update (environment, func_name, macro);

  return func_name;
}
