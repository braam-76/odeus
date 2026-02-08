#include "builtins/constrol_flow.h"

#include "core/eval.h"
#include "core/value.h"

Value *
builtin_eq (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 2)
    return val_error ("eq: expects exactly two arguments");

  Value *first = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (first);
  Value *second = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (second);

  if (first->type != second->type)
    return val_nil ();

  switch (first->type)
    {
    case VALUE_NIL:
    case VALUE_BUILTIN:
    case VALUE_SYMBOL:
      return (first == second) ? val_t () : val_nil ();
    default:
      return val_nil ();
    }

  return (first == second) ? val_t () : val_nil ();
}

Value *
builtin_if (Environment *environment, Value *args)
{
  if (arguments_length (args) != 3)
    return val_error (
        "if: expects exactly 3 arguments (if [condition] [then] [else])");

  Value *cond = evaluate_expression (environment, CAR (args));
  ERROR_OUT (cond);

  if (!IS_NULL (cond))
    return evaluate_expression (environment, CADR (args));
  else
    return evaluate_expression (environment, CADR (CDR (args)));
}

Value *
builtin_and (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) <= 0)
    return val_error ("ERROR: and expects at levalue 1 argument\n");

  while (arguments->type == VALUE_CONS)
    {
      Value *current_condition
          = evaluate_expression (environment, CAR (arguments));

      if (IS_NULL (current_condition))
        return val_nil ();

      arguments = CDR (arguments);
    }

  return val_t ();
}

Value *
builtin_or (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) <= 0)
    return val_error ("ERROR: or expects at levalue 1 argument\n");

  while (arguments->type == VALUE_CONS)
    {
      Value *current_condition
          = evaluate_expression (environment, CAR (arguments));

      if (!IS_NULL (current_condition))
        return val_t ();

      arguments = CDR (arguments);
    }

  return val_nil ();
}
