#include "odeus_forms.h"
#include "odeus_eval.h"

AST *
builtin_define (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return make_error ("ERROR: define expects (symbol value_expression)\n");

  AST *symbol = CAR (arguments);
  if (symbol->type != AST_SYMBOL)
    return make_error ("ERROR: define: first argument must be symbol\n");

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
builtin_print (AST *environment, AST *arguments)
{
  while (arguments->type == AST_CONS)
    {
      AST *value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (value);

      ast_print (value);
      printf (" ");

      arguments = CDR (arguments);
    }

  printf ("\n");
  return nil ();
}
