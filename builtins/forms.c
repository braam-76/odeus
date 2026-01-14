#include "builtins/forms.h"

static int
is_reserved_symbol (const char *name)
{
  return strcmp (name, "unquote") == 0 || strcmp (name, "unquote-splicing") == 0
         || strcmp (name, "quasiquote") == 0;
}

AST *
builtin_define (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return make_error ("ERROR: define expects (symbol value_expression)\n");

  AST *symbol = CAR (arguments);
  if (symbol->type != AST_SYMBOL)
    return make_error ("ERROR: define: first argument must be symbol\n");

  if (is_reserved_symbol (symbol->as.SYMBOL))
    return make_error ("ERROR: cannot redefine reserved syntax\n");

  AST *value = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (value);

  environment_set (environment, symbol, value);
  return symbol;
}

AST *
builtin_begin (AST *environment, AST *arguments)
{
  AST *last_value = nil ();

  while (arguments->type == AST_CONS)
    {
      last_value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (last_value);
      arguments = CDR (arguments);
    }

  return last_value;
}

AST *
builtin_eval (AST *env, AST *args)
{
  if (IS_NULL (args) || !IS_NULL (CDR (args)))
    return make_error ("eval expects exactly one argument");

  AST *expr = CAR (args);

  AST *expr_to_eval = evaluate_expression (env, expr);
  ERROR_OUT (expr_to_eval);

  // If it's a quote node, unwrap it
  if (expr_to_eval->type == AST_QUOTE)
    expr_to_eval = expr_to_eval->as.QUOTE.EXPR;

  return evaluate_expression (env, expr_to_eval);
}

AST *
builtin_quote (AST *environment, AST *arguments)
{
  (void)environment;

  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("ERROR: quote expects one argument\n");

  return CAR (arguments);
}

AST *
builtin_quasiquote (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("quasiquote expects exactly one argument");

  AST *res = expand_quasiquote (CAR (arguments), 1);

  printf ("Quasiquote expanded to: ");
  ast_print (res);
  printf ("\n");

  return res;
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
