#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENV_CAPACITY 4096

// forward declarations to resolve cycling includes
typedef struct Value Val;
Val *val_error (const char *message);

typedef struct Binding
{
  Val *key;
  Val *value;
} Binding;

typedef struct Environment
{
  struct Environment *parent;
  Binding bindings[ENV_CAPACITY];
  size_t bindings_size;
} Env;

Env *env_init (Env *parent);

void env_set (Env *env, Val *symbol, Val *value);
Val *env_get (Env *env, Val *symbol);
void env_update (Env *env, Val *symbol, Val *value);

#endif // ENVIRONMENT_H_
