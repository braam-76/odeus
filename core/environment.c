#include "core/environment.h"

void
environment_set (Val *environment, Val *symbol, Val *value)
{
  Val *variables = CDR (environment);

  while (variables->type == VALUE_CONS)
    {
      Val *pair = CAR (variables);
      Val *key_symbol = CAR (pair);

      if (key_symbol == symbol)
        {
          pair->as.CONS.CDR = value;
          return;
        }

      variables = CDR (variables);
    }

  Val *pair = make_cons (symbol, value);
  environment->as.CONS.CDR = make_cons (pair, CDR (environment));
}

void
environment_update (Val *env, Val *symbol, Val *value)
{
  Val *bindings = env->as.CONS.CDR;

  while (bindings->type == VALUE_CONS)
    {
      Val *binding = CAR (bindings);

      if (CAR (binding) == symbol)
        {
          binding->as.CONS.CDR = value;
          return;
        }

      bindings = CDR (bindings);
    }

  environment_set (env, symbol, value);
}

Val *
environment_get (Val *environment, Val *symbol)
{
  Val *variables = CDR (environment);
  Val *parent_environment = CAR (environment);

  while (variables->type != VALUE_NIL)
    {
      Val *pair = CAR (variables);
      Val *key_symbol = CAR (pair);

      if (key_symbol == symbol)
        return CDR (pair);

      variables = CDR (variables);
    }

  if (parent_environment->type == VALUE_NIL)
    {
      char buf[256];
      snprintf (buf, sizeof (buf), "Unbound symbol: %s", symbol->as.SYMBOL);
      return make_error (buf);
    }

  return environment_get (parent_environment, symbol);
}
