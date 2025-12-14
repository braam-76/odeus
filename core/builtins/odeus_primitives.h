#ifndef ODEUS_PRIMITIVES_H_
#define ODEUS_PRIMITIVES_H_

#include "../odeus_parser.h"
#include "../odeus_environment.h"

AST *builtin_quote (AST *environment, AST *arguments);
AST *builtin_atom (AST *environment, AST *arguments);
AST *builtin_null (AST *environment, AST *arguments);
AST *builtin_eq (AST *environment, AST *arguments);
AST *builtin_cons (AST *environment, AST *arguments);
AST *builtin_lambda (AST *environment, AST *arguments);

#endif // ODEUS_PRIMITIVES_H_
