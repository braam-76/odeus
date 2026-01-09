#ifndef EVAL_H_
#define EVAL_H_

#include "environment.h"
#include "ast.h"

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

#endif // EVAL_H_
