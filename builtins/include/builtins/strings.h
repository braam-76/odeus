#ifndef STRINGS_H_
#define STRINGS_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_concat (Val *environment, Val *arguments);
Val *builtin_string_length (Val *environment, Val *arguments);
Val *builtin_substring (Val *environment, Val *arguments);
Val *builtin_string_to_symbol (Val *environment, Val *arguments);
Val *builtin_symbol_to_string (Val *environment, Val *arguments);

#endif // STRINGS_H_
