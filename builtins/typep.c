#include "builtins/typep.h"

AST *
builtin_is_atom (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("atom?: expects one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));

  return expression->type != AST_CONS ? t () : nil ();
}

AST *
builtin_is_symbol (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("symbol?: expects one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));

  return expression->type == AST_SYMBOL ? t () : nil ();
}

AST *
builtin_is_string (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("string?: expects one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));

  return expression->type == AST_STRING ? t () : nil ();
}

AST *
builtin_is_number (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("number?: expects one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));

  return (expression->type == AST_INTEGER || expression->type == AST_FLOAT) ? t () : nil ();
}

AST *
builtin_is_integer (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("integer?: expects one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));

  return expression->type == AST_INTEGER ? t () : nil ();
}

AST *
builtin_is_float (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("float?: expects one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));

  return expression->type == AST_FLOAT ? t () : nil ();
}

AST *
builtin_is_cons (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("cons?: expects one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));

  return expression->type == AST_CONS ? t () : nil ();
}

AST *
builtin_is_function (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("function?: expects one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));

  return (expression->type == AST_BUILTIN_NORMAL || expression->type == AST_BUILTIN_SPECIAL
          || expression->type == AST_LAMBDA)
             ? t ()
             : nil ();
}
