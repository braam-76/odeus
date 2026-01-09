#include "eval.h"

void
environment_set (AST *environment, AST *symbol, AST *value)
{
  AST *variables = CDR (environment);

  while (variables->type != AST_NIL)
    {
      AST *pair = CAR (variables); /* (symbol . value) */
      AST *key_symbol = CAR (pair);

      if (key_symbol == symbol)
        {
          CDR (pair) = value;
          return;
        }

      variables = CDR (variables);
    }

  /* Add new (symbol . value) pair at beginning */
  AST *pair = make_cons (symbol, value);
  CDR (environment) = make_cons (pair, CDR (environment));
}

AST *
environment_get (AST *environment, AST *symbol)
{
  AST *variables = CDR (environment);
  AST *parent_environment = CAR (environment);

  while (variables->type != AST_NIL)
    {
      AST *pair = CAR (variables);
      AST *key_symbol = CAR (pair);

      if (strcmp (key_symbol->as.SYMBOL, symbol->as.SYMBOL) == 0)
        return CDR (pair);

      variables = CDR (variables);
    }

  if (parent_environment->type == AST_NIL)
    {
      char buf[256];
      snprintf (buf, sizeof (buf), "Unbound symbol: %s", symbol->as.SYMBOL);
      return make_error (buf);
    }

  return environment_get (parent_environment, symbol);
}
