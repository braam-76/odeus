#ifndef BASIC_MATH_H_
#define BASIC_MATH_H_

#include "../core/ast.h"

AST *builtin_add (AST *environment, AST *arguments);
AST *builtin_sub (AST *environment, AST *arguments);
AST *builtin_mul (AST *environment, AST *arguments);
AST *builtin_div (AST *environment, AST *arguments);

#endif // BASIC_MATH_H_
