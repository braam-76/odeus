#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "../core/ast.h"

AST *builtin_quote (AST *environment, AST *arguments);
AST *builtin_atom (AST *environment, AST *arguments);
AST *builtin_null (AST *environment, AST *arguments);
AST *builtin_eq (AST *environment, AST *arguments);
AST *builtin_cons (AST *environment, AST *arguments);
AST *builtin_lambda (AST *environment, AST *arguments);

#endif // PRIMITIVES_H_
