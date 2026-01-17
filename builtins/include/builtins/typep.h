#ifndef TYPE_PREDICATES_H_
#define TYPE_PREDICATES_H_

#include "core/ast.h"
#include "core/eval.h"

AST *builtin_is_atom (AST *environment, AST *arguments);
AST *builtin_is_symbol (AST *environment, AST *arguments);
AST *builtin_is_string (AST *environment, AST *arguments);
AST *builtin_is_number (AST *environment, AST *arguments);
AST *builtin_is_integer (AST *environment, AST *arguments);
AST *builtin_is_float (AST *environment, AST *arguments);
AST *builtin_is_list (AST *environment, AST *arguments);
AST *builtin_is_cons (AST *environment, AST *arguments);
AST *builtin_is_function (AST *environment, AST *arguments);

#endif // TYPE_PREDICATES_H_
