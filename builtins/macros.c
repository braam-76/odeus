#include "builtins/macros.h"
#include "core/eval.h"

Val *
builtin_macro (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("macro: expects parameter list");

  Val *parameters = CAR (arguments);
  Val *body = CDR (arguments);

  Val *macro = malloc (sizeof (Val));
  macro->type = VALUE_MACRO;
  macro->as.MACRO.parameters = parameters;
  macro->as.MACRO.body = body;
  macro->as.MACRO.environment = environment;

  return macro;
}

Val *
builtin_defmacro (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return make_error ("defmacro: expects (macro-name args) expr");

  Val *to_be_defined = CAR (arguments);

  if (to_be_defined->type == VALUE_CONS)

    if (IS_NULL (to_be_defined))
      return make_error ("defmacro: macro definition cannot be empty");

  Val *func_name = CAR (to_be_defined);
  if (func_name->type != VALUE_SYMBOL)
    return make_error ("defmacro: macro name must be a symbol");

  Val *current = environment_get (environment, func_name);
  if (current->type != VALUE_ERROR)
    return make_error ("defmacro: symbol already defined");

  Val *params = CDR (to_be_defined);

  // Initialize with nil first
  environment_set (environment, func_name, nil ());

  Val *macro_args = make_cons (params, CDR (arguments));
  Val *macro = builtin_macro (environment, macro_args);
  ERROR_OUT (macro);

  // Update the environment with the macro
  environment_update (environment, func_name, macro);

  return func_name;
}

Val *
builtin_macroexpand (Val *environment, Val *arguments)
{
    if (IS_NULL(arguments))
        return make_error("macroexpand: expected 1 argument");

    // Get the expression (e.g., strip the quote from '(my-macro ...))
    Val *expr = macro_expand_expression(environment, CAR(arguments));
    ERROR_OUT(expr);

    return expr;
}
