#ifndef EVAL_H_
#define EVAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/ast.h"
#include "core/value.h"
#include "core/environment.h"

Val *evaluate_expression (Env *environment, Val *expression);
Val *apply (Env *environment, Val *function, Val *arguments);
Val *macro_expand_expression (Env *environment, Val *expr);

#define ERROR_OUT(x)                                                          \
  do                                                                          \
    {                                                                         \
      if ((x)->type == VALUE_ERROR)                                             \
        return (x);                                                           \
    }                                                                         \
  while (0)

int arguments_length (Val *arguments);

#endif // EVAL_H_
