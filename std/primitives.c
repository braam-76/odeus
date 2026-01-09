#include "primitives.h"

AST *
builtin_quote (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("ERROR: quote expects one argument\n");

  return CAR (arguments);
}

AST *
builtin_atom (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("ERROR: atom expects one argument\n");

  return CAR (arguments)->type != AST_CONS ? t () : nil ();
}

AST *
builtin_eq (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments) || IS_NULL (CADR (arguments)) || !IS_NULL (CDDR (arguments)))
    return make_error ("ERROR: eq expects two argument\n");

  AST *first_argument = CAR (arguments);
  AST *second_argument = CADR (arguments);

  return first_argument == second_argument ? t () : nil ();
}

AST *
builtin_cons (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments) || IS_NULL (CADR (arguments)) || !IS_NULL (CDDR (arguments)))
    return make_error ("ERROR: cons expects two argument\n");

  AST *first_argument = CAR (arguments);
  AST *second_argument = CADR (arguments);

  return make_cons (first_argument, second_argument);
}

AST *
builtin_null (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("ERROR: null expects one argument\n");

  return CAR (arguments) == nil () ? t () : nil ();
}

AST *
builtin_lambda (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("ERROR: lambda expects at least parameter list\n");

  AST *parameters = CAR (arguments);
  AST *body = CDR (arguments);

  AST *lambda = malloc (sizeof (AST));
  lambda->type = AST_LAMBDA;
  lambda->as.LAMBDA.parameters = parameters;
  lambda->as.LAMBDA.body = body;
  lambda->as.LAMBDA.environment = environment;

  return lambda;
}
