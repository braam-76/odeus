#ifndef MATH_H_
#define MATH_H_

#include <math.h>

#include "core/value.h"
#include "core/eval.h"

// Essentials
Val *builtin_add (Env *environment, Val *arguments);
Val *builtin_sub (Env *environment, Val *arguments);
Val *builtin_mul (Env *environment, Val *arguments);
Val *builtin_div (Env *environment, Val *arguments);

// More math
Val *builtin_mod (Env *environment, Val *arguments);
Val *builtin_expt (Env *environment, Val *arguments);
Val *builtin_sqrt (Env *environment, Val *arguments);
Val *builtin_abs (Env *environment, Val *arguments);
Val *builtin_floor (Env *environment, Val *arguments);
Val *builtin_ceil (Env *environment, Val *arguments);
Val *builtin_round (Env *environment, Val *arguments);

// Numeric comparison
Val *builtin_num_eq (Env *environment, Val *arguments);
Val *builtin_num_gt (Env *environment, Val *arguments);
Val *builtin_num_lt (Env *environment, Val *arguments);
Val *builtin_num_gte (Env *environment, Val *arguments);
Val *builtin_num_lte (Env *environment, Val *arguments);

#endif // MATH_H_
