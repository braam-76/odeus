#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "core/value.h"

void environment_set (Val *env, Val *symbol, Val *value);
Val *environment_get (Val *env, Val *symbol);
void environment_update (Val *env, Val *symbol, Val *value);

#endif // ENVIRONMENT_H_
