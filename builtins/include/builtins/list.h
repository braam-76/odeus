#ifndef LIST_OPERATIONS_H_
#define LIST_OPERATIONS_H_

#include "core/ast.h"
#include "core/eval.h"

AST *builtin_cons (AST *environment, AST *arguments);
AST *builtin_list (AST *environment, AST *arguments);
AST *builtin_append (AST *environment, AST *arguments);

AST *builtin_car(AST* environment, AST* arguments);
AST *builtin_cdr(AST* environment, AST* arguments);
AST *builtin_set_car(AST* environment, AST* arguments);
AST *builtin_set_cdr(AST* environment, AST* arguments);
AST *builtin_length(AST* environment, AST* arguments);
AST *builtin_reverse(AST* environment, AST* arguments);
AST *builtin_map(AST* environment, AST* arguments);
AST *builtin_filter(AST* environment, AST* arguments);
AST *builtin_foldr(AST* environment, AST* arguments);



#endif // LIST_OPERATIONS_H_
