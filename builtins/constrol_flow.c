#include "builtins/constrol_flow.h"

#include "core/value.h"
#include "core/eval.h"

Val *
builtin_eq (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("ERROR: eq expects exactly two arguments");

  Val *first_value = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (first_value);
  Val *second_value = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (second_value);

  return (first_value == second_value) ? t () : nil ();
}

Val *
builtin_if (Val *env, Val *args)
{
  if (arguments_length (args) != 3)
    return make_error (
        "if: expects exactly 3 arguments (if [condition] [then] [else])");

  Val *cond = evaluate_expression (env, CAR (args));
  ERROR_OUT (cond);

  if (!IS_NULL (cond))
    return evaluate_expression (env, CADR (args));
  else
    return evaluate_expression (env, CADR (CDR (args)));
}

Val *
builtin_and (Val *environment, Val *arguments)
{
  if (arguments_length(arguments) <= 0)
    return make_error ("ERROR: and expects at levalue 1 argument\n");

  while (arguments->type == VALUE_CONS)
    {
      Val *current_condition
          = evaluate_expression (environment, CAR (arguments));

      if (IS_NULL (current_condition))
        return nil ();

      arguments = CDR (arguments);
    }

  return t ();
}

Val *
builtin_or (Val *environment, Val *arguments)
{
  if (arguments_length(arguments) <= 0)
    return make_error ("ERROR: or expects at levalue 1 argument\n");

  while (arguments->type == VALUE_CONS)
    {
      Val *current_condition
          = evaluate_expression (environment, CAR (arguments));

      if (!IS_NULL (current_condition))
        return t ();

      arguments = CDR (arguments);
    }

  return nil ();
}
