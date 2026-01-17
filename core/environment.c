#include "core/environment.h"

void
environment_set (AST *environment, AST *symbol, AST *value)
{
  AST *variables = CDR (environment);

  while (variables->type == AST_CONS)
    {
      AST *pair = CAR (variables);
      AST *key_symbol = CAR (pair);

      if (key_symbol == symbol)
        {
          pair->as.CONS.CDR = value;
          return;
        }

      variables = CDR (variables);
    }

  AST *pair = make_cons (symbol, value);
  environment->as.CONS.CDR = make_cons (pair, CDR (environment));
}

void
environment_update (AST *env, AST *symbol, AST *value)
{
  AST *bindings = env->as.CONS.CDR;

  while (bindings->type == AST_CONS)
    {
      AST *binding = CAR (bindings);

      if (CAR (binding) == symbol)
        {
          binding->as.CONS.CDR = value;
          return;
        }

      bindings = CDR (bindings);
    }

  environment_set (env, symbol, value);
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

      if (key_symbol == symbol)
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
