#ifndef EVAL_H_
#define EVAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/ast.h"
#include "core/environment.h"

AST *evaluate_expression (AST *environment, AST *expression);
AST *apply (AST *function, AST *environment, AST *arguments);
AST *macro_expand_expression (AST *environment, AST *expr);

#define ERROR_OUT(x)                                                          \
  do                                                                          \
    {                                                                         \
      if ((x)->type == AST_ERROR)                                             \
        return (x);                                                           \
    }                                                                         \
  while (0)

int arguments_length (AST *arguments);

#endif // EVAL_H_
