#ifndef MACROS_H_
#define MACROS_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_defmacro (Env *environment, Val *arguments);
Val *builtin_macro (Env *environment, Val *arguments);
Val *builtin_macroexpand (Env *environment, Val *arguments);

#endif // MACROS_H_
