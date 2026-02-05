#ifndef EVAL_H_
#define EVAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/ast.h"
#include "core/value.h"
#include "core/environment.h"

Value *evaluate_expression (Environment *environment, Value *expression);
Value *apply (Environment *environment, Value *function, Value *arguments);
Value *macro_expand_expression (Environment *environment, Value *expr);

#define ERROR_OUT(x)                                                          \
  do                                                                          \
    {                                                                         \
      if ((x)->type == VALUE_ERROR)                                             \
        return (x);                                                           \
    }                                                                         \
  while (0)

int arguments_length (Value *arguments);

#endif // EVAL_H_
