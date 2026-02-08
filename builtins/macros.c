#include "builtins/macros.h"
#include "core/eval.h"

#include <gc.h>

Value *
builtin_macro (Environment *environment, Value *arguments)
{
  if (IS_NULL (arguments))
    return val_error ("macro: expects parameter list");

  Value *parameters = CAR (arguments);
  Value *body = CDR (arguments);

  Value *macro = GC_malloc (sizeof (Value));
  macro->type = VALUE_MACRO;
  macro->as.MACRO.parameters = parameters;
  macro->as.MACRO.body = body;
  macro->as.MACRO.environment = environment;

  return macro;
}

Value *
builtin_defmacro (Environment *environment, Value *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return val_error ("defmacro: expects (macro-name args) expr");

  Value *to_be_defined = CAR (arguments);

  if (to_be_defined->type == VALUE_CONS)

    if (IS_NULL (to_be_defined))
      return val_error ("defmacro: macro definition cannot be empty");

  Value *func_name = CAR (to_be_defined);
  if (func_name->type != VALUE_SYMBOL)
    return val_error ("defmacro: macro name must be a symbol");

  Value *current = env_get (environment, func_name);
  if (current->type != VALUE_ERROR)
    return val_error ("defmacro: symbol already defined");

  Value *params = CDR (to_be_defined);

  // Initialize with nil first
  env_set (environment, func_name, val_nil (), func_name->meta);

  Value *macro_args = val_cons (params, CDR (arguments));
  Value *macro = builtin_macro (environment, macro_args);
  ERROR_OUT (macro);

  // Update the environment with the macro
  env_update (environment, func_name, macro, func_name->meta);

  return func_name;
}

Value *
builtin_macroexpand (Environment *environment, Value *arguments)
{
    if (IS_NULL(arguments))
        return val_error("macroexpand: expected 1 argument");

    // Get the expression (e.g., strip the quote from '(my-macro ...))
    Value *expr = macro_expand_expression(environment, CAR(arguments));
    ERROR_OUT(expr);

    return expr;
}
