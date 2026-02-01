#ifndef STRINGS_H_
#define STRINGS_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_concat (Env *environment, Val *arguments);
Val *builtin_string_length (Env *environment, Val *arguments);
Val *builtin_substring (Env *environment, Val *arguments);
Val *builtin_string_to_symbol (Env *environment, Val *arguments);
Val *builtin_symbol_to_string (Env *environment, Val *arguments);

#endif // STRINGS_H_
