#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "core/ast.h"

void environment_set (AST *env, AST *symbol, AST *value);
AST *environment_get (AST *env, AST *symbol);

#endif // ENVIRONMENT_H_
