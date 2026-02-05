#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/meta.h"

#define ENV_CAPACITY 4096

// forward declarations to resolve cycling includes
typedef struct Value Value;
Value *val_error (const char *message, ...);

typedef struct
{
  Value *key;

  Value *value;
  Meta meta;
} Binding;

typedef struct Env
{
  struct Env *parent;
  Binding bindings[ENV_CAPACITY];
  size_t bindings_size;
} Environment;

Environment *env_init (Environment *parent);

void env_set (Environment *env, Value *symbol, Value *value, Meta meta);
Value *env_get (Environment *env, Value *symbol);
Binding *env_get_binding (Environment *env, Value *symbol);
void env_update (Environment *env, Value *symbol, Value *value, Meta meta);

#endif // ENVIRONMENT_H_
