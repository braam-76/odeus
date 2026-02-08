#include "core/environment.h"
#include "core/value.h"
#include <gc/gc.h>

Environment *
env_init (Environment *parent)
{
  Environment *env = GC_malloc (sizeof (Environment));
  memset (env, 0, sizeof (Environment));
  env->parent = parent;
  env->bindings_size = 0;
  return env;
}

void
env_set (Environment *environment, Value *symbol, Value *value, Meta meta)
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
env_update (Environment *environment, Value *symbol, Value *value, Meta meta)
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

Value *
env_get (Environment *environment, Value *symbol)
{
  for (int i = 0; i < environment->bindings_size; i++)
    if (environment->bindings[i].key->as.SYMBOL == symbol->as.SYMBOL)
      return environment->bindings[i].value;

  if (environment->parent == NULL)
    {
      char buf[256];
      snprintf (
          buf, sizeof (buf), "Unbound symbol: %s",
          symbol->as.SYMBOL); // need to somehow provide symbol as character
      return val_error (buf);
    }

  return env_get (environment->parent, symbol);
}

Binding *
env_get_binding (Environment *environment, Value *symbol)
{
  for (int i = 0; i < environment->bindings_size; i++)
    if (environment->bindings[i].key->as.SYMBOL == symbol->as.SYMBOL)
      return &environment->bindings[i];

  if (environment->parent == NULL)
    return NULL;

  return env_get_binding (environment->parent, symbol);
}
