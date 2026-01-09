#ifndef ODEUS_FORMS_H_
#define ODEUS_FORMS_H_

#include "odeus_parser.h"

AST *builtin_print (AST *environment, AST *arguments);
AST *builtin_define (AST *environment, AST *arguments);
AST *builtin_begin (AST *environment, AST *arguments);


#endif // ODEUS_FORMS_H_
