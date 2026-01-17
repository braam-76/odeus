#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "core/ast.h"

void environment_set (AST *env, AST *symbol, AST *value);
AST *environment_get (AST *env, AST *symbol);
void environment_update (AST *env, AST *symbol, AST *value);

#endif // ENVIRONMENT_H_
