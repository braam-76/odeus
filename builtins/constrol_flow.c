#include "builtins/constrol_flow.h"

#include "core/ast.h"
#include "core/eval.h"

AST *
builtin_eq (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("ERROR: eq expects exactly two arguments");

  AST *first_value = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (first_value);
  AST *second_value = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (second_value);

  return (first_value == second_value) ? t () : nil ();
}

AST *
builtin_null (AST *environment, AST *arguments)
{
  if (arguments_length(arguments) != 1)
    return make_error ("ERROR: null expects one argument\n");

  return evaluate_expression (environment, CAR (arguments)) == nil () ? t ()
                                                                      : nil ();
}

AST *
builtin_if (AST *env, AST *args)
{
  if (arguments_length (args) != 3)
    return make_error (
        "if: expects exactly 3 arguments (if [condition] [then] [else])");

  AST *cond = evaluate_expression (env, CAR (args));
  ERROR_OUT (cond);

  if (!IS_NULL (cond))
    return evaluate_expression (env, CADR (args));
  else
    return evaluate_expression (env, CADR (CDR (args)));
}

AST *
builtin_and (AST *environment, AST *arguments)
{
  if (arguments_length(arguments) <= 0)
    return make_error ("ERROR: and expects at least 1 argument\n");

  while (arguments->type == AST_CONS)
    {
      AST *current_condition
          = evaluate_expression (environment, CAR (arguments));

      if (IS_NULL (current_condition))
        return nil ();

      arguments = CDR (arguments);
    }

  return t ();
}

AST *
builtin_or (AST *environment, AST *arguments)
{
  if (arguments_length(arguments) <= 0)
    return make_error ("ERROR: or expects at least 1 argument\n");

  while (arguments->type == AST_CONS)
    {
      AST *current_condition
          = evaluate_expression (environment, CAR (arguments));

      if (!IS_NULL (current_condition))
        return t ();

      arguments = CDR (arguments);
    }

  return nil ();
}
