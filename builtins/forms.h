#ifndef FORMS_H_
#define FORMS_H_

#include "ast.h"

AST *builtin_print (AST *environment, AST *arguments);
AST *builtin_define (AST *environment, AST *arguments);
AST *builtin_begin (AST *environment, AST *arguments);


#endif // FORMS_H_
