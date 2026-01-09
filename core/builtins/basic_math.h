#ifndef ODEUS_BASIC_MATH_H_
#define ODEUS_BASIC_MATH_H_

#include "../odeus_ast.h"

AST *builtin_add (AST *environment, AST *arguments);
AST *builtin_sub (AST *environment, AST *arguments);
AST *builtin_mul (AST *environment, AST *arguments);
AST *builtin_div (AST *environment, AST *arguments);

#endif // ODEUS_BASIC_MATH_H_
