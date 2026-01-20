#ifndef FORMS_H_
#define FORMS_H_

#include "core/ast.h"
#include "core/eval.h"

AST *builtin_define (AST *environment, AST *arguments);
AST *builtin_set (AST *environment, AST *arguments);
AST *builtin_let (AST *environment, AST *arguments);
AST *builtin_let_star (AST *environment, AST *arguments);
AST *builtin_letrec (AST *environment, AST *arguments);

AST *builtin_begin (AST *environment, AST *arguments);
AST *builtin_eval (AST *environment, AST *arguments);

AST *builtin_quote (AST *environment, AST *arguments);
AST *builtin_quasiquote (AST *environment, AST *arguments);

AST *builtin_lambda (AST *environment, AST *arguments);
AST *builtin_macro (AST *environment, AST *arguments);

#endif // FORMS_H_
