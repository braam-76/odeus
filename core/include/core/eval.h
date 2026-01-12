#ifndef EVAL_H_
#define EVAL_H_

#include "core/environment.h"
#include "core/ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AST *evaluate_expression (AST *environment, AST *expression);

#define ERROR_OUT(x)                                                                               \
  do                                                                                               \
    {                                                                                              \
      if ((x)->type == AST_ERROR)                                                                  \
        return (x);                                                                                \
    }                                                                                              \
  while (0)

AST *expand_quasiquote (AST *expr, int depth);
int arguments_length(AST*arguments);

#endif // EVAL_H_
