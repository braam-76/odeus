#ifndef ODEUS_EVAL_H_
#define ODEUS_EVAL_H_

#include "odeus_environment.h"
#include "odeus_parser.h"

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

#endif // ODEUS_EVAL_H_
