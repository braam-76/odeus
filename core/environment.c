#include "core/environment.h"
#include "core/value.h"

Env *
env_init (Env *parent)
{
  Env *env = malloc (sizeof (Env));
  env->parent = parent;
  env->bindings_size = 0;
  return env;
}

void
env_set (Env *environment, Val *symbol, Val *value, Meta meta)
{
  for (int i = 0; i < environment->bindings_size; i++)
    if (environment->bindings[i].key == symbol)
      {
        environment->bindings[i].value = value;
        environment->bindings[i].meta = meta;
        return;
      }

  environment->bindings[environment->bindings_size].key = symbol;
  environment->bindings[environment->bindings_size].value = value;
  environment->bindings[environment->bindings_size].meta = meta;
  environment->bindings_size++;
}

void
env_update (Env *environment, Val *symbol, Val *value, Meta meta)
{
  for (int i = 0; i < environment->bindings_size; i++)
    if (environment->bindings[i].key == symbol)
      {
        environment->bindings[i].value = value;
        environment->bindings[i].meta = meta;
        return;
      }

  env_set (environment, symbol, value, meta);
}

Val *
env_get (Env *environment, Val *symbol)
{
  for (int i = 0; i < environment->bindings_size; i++)
    if (environment->bindings[i].key->as.SYMBOL == symbol->as.SYMBOL)
      return environment->bindings[i].value;

  if (environment->parent == NULL)
    {
      char buf[256];
      snprintf (buf, sizeof (buf), "Unbound symbol: %s", symbol->as.SYMBOL); // need to somehow provide symbol as character
      return val_error (buf);
    }

  return env_get (environment->parent, symbol);
}
