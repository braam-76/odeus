#ifndef MATH_H_
#define MATH_H_

#include <math.h>

#include "core/value.h"
#include "core/eval.h"

// Essentials
Val *builtin_add (Val *environment, Val *arguments);
Val *builtin_sub (Val *environment, Val *arguments);
Val *builtin_mul (Val *environment, Val *arguments);
Val *builtin_div (Val *environment, Val *arguments);

// More math
Val *builtin_mod (Val *environment, Val *arguments);
Val *builtin_expt (Val *environment, Val *arguments);
Val *builtin_sqrt (Val *environment, Val *arguments);
Val *builtin_abs (Val *environment, Val *arguments);
Val *builtin_floor (Val *environment, Val *arguments);
Val *builtin_ceil (Val *environment, Val *arguments);
Val *builtin_round (Val *environment, Val *arguments);

// Numeric comparison
Val *builtin_num_eq (Val *environment, Val *arguments);
Val *builtin_num_gt (Val *environment, Val *arguments);
Val *builtin_num_lt (Val *environment, Val *arguments);
Val *builtin_num_gte (Val *environment, Val *arguments);
Val *builtin_num_lte (Val *environment, Val *arguments);

#endif // MATH_H_
