#ifndef MACROS_H_
#define MACROS_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_defmacro (Val *environment, Val *arguments);
Val *builtin_macro (Val *environment, Val *arguments);
Val *builtin_macroexpand (Val *environment, Val *arguments);

#endif // MACROS_H_
