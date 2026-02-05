#ifndef MACROS_H_
#define MACROS_H_

#include "core/value.h"
#include "core/eval.h"

Value *builtin_defmacro (Environment *environment, Value *arguments);
Value *builtin_macro (Environment *environment, Value *arguments);
Value *builtin_macroexpand (Environment *environment, Value *arguments);

#endif // MACROS_H_
