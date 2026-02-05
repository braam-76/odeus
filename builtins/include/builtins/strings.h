#ifndef STRINGS_H_
#define STRINGS_H_

#include "core/value.h"
#include "core/eval.h"

Value *builtin_concat (Environment *environment, Value *arguments);
Value *builtin_string_length (Environment *environment, Value *arguments);
Value *builtin_substring (Environment *environment, Value *arguments);
Value *builtin_string_to_symbol (Environment *environment, Value *arguments);
Value *builtin_symbol_to_string (Environment *environment, Value *arguments);

#endif // STRINGS_H_
