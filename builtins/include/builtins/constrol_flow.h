#ifndef CONSTROL_FLOW_H_
#define CONSTROL_FLOW_H_

#include "core/ast.h"
#include "core/eval.h"

AST *builtin_eq (AST *environment, AST *arguments);
AST *builtin_null (AST *environment, AST *arguments);
AST *builtin_if (AST *environment, AST *arguments);
AST *builtin_and (AST *environment, AST *arguments);
AST *builtin_or (AST *environment, AST *arguments);

#endif // CONSTROL_FLOW_H_
