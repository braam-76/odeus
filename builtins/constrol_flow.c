#include "builtins/constrol_flow.h"

#include "core/ast.h"
#include "core/eval.h"

AST *
builtin_eq (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CADR (arguments))
      || !IS_NULL (CDDR (arguments)))
    return make_error ("ERROR: eq expects two argument\n");

  AST *first_argument = evaluate_expression (environment, CAR (arguments));
  AST *second_argument = evaluate_expression (environment, CADR (arguments));

  return first_argument == second_argument ? t () : nil ();
}

AST *
builtin_null (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
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
  if (IS_NULL (arguments))
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
  if (IS_NULL (arguments))
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
