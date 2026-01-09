#ifndef ODEUS_ENVIRONMENT_H_
#define ODEUS_ENVIRONMENT_H_

#include "odeus_ast.h"

void environment_set (AST *env, AST *symbol, AST *value);
AST *environment_get (AST *env, AST *symbol);

#endif // ODEUS_ENVIRONMENT_H_
