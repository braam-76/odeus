#ifndef MATH_H_
#define MATH_H_

#include <math.h>

#include "core/ast.h"
#include "core/eval.h"

// Essentials
AST *builtin_add (AST *environment, AST *arguments);
AST *builtin_sub (AST *environment, AST *arguments);
AST *builtin_mul (AST *environment, AST *arguments);
AST *builtin_div (AST *environment, AST *arguments);

// More math
AST *builtin_mod (AST *environment, AST *arguments);
AST *builtin_expt (AST *environment, AST *arguments);
AST *builtin_sqrt (AST *environment, AST *arguments);
AST *builtin_abs (AST *environment, AST *arguments);
AST *builtin_floor (AST *environment, AST *arguments);
AST *builtin_ceil (AST *environment, AST *arguments);
AST *builtin_round (AST *environment, AST *arguments);

// Numeric comparison
AST *builtin_num_eq (AST *environment, AST *arguments);
AST *builtin_num_gt (AST *environment, AST *arguments);
AST *builtin_num_lt (AST *environment, AST *arguments);
AST *builtin_num_gte (AST *environment, AST *arguments);
AST *builtin_num_lte (AST *environment, AST *arguments);

#endif // MATH_H_
