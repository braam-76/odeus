#ifndef MACROS_H_
#define MACROS_H_

#include "core/ast.h"
#include "core/eval.h"

AST *builtin_defmacro (AST *environment, AST *arguments);
AST *builtin_macro (AST *environment, AST *arguments);
AST *builtin_macroexpand (AST *environment, AST *arguments);

#endif // MACROS_H_
