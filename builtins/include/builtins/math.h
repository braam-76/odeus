#ifndef MATH_H_
#define MATH_H_

#include <math.h>

#include "core/value.h"
#include "core/eval.h"

// Essentials
Value *builtin_add (Environment *environment, Value *arguments);
Value *builtin_sub (Environment *environment, Value *arguments);
Value *builtin_mul (Environment *environment, Value *arguments);
Value *builtin_div (Environment *environment, Value *arguments);

// More math
Value *builtin_mod (Environment *environment, Value *arguments);
Value *builtin_expt (Environment *environment, Value *arguments);
Value *builtin_sqrt (Environment *environment, Value *arguments);
Value *builtin_abs (Environment *environment, Value *arguments);
Value *builtin_floor (Environment *environment, Value *arguments);
Value *builtin_ceil (Environment *environment, Value *arguments);
Value *builtin_round (Environment *environment, Value *arguments);

// Numeric comparison
Value *builtin_num_eq (Environment *environment, Value *arguments);
Value *builtin_num_gt (Environment *environment, Value *arguments);
Value *builtin_num_lt (Environment *environment, Value *arguments);
Value *builtin_num_gte (Environment *environment, Value *arguments);
Value *builtin_num_lte (Environment *environment, Value *arguments);

#endif // MATH_H_
