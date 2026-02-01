#ifndef STDIO_H_
#define STDIO_H_

#include <stdio.h>

#include "core/value.h"
#include "core/eval.h"
#include "core/lexer.h"
#include "core/parser.h"

Val *builtin_dump (Env *environment, Val *arguments);
Val *builtin_read (Env *environment, Val *arguments);
Val *builtin_read_file (Env *environment, Val *arguments);
Val *builtin_load_file (Env *environment, Val *arguments);
Val *builtin_file_to_string (Env *environment, Val *arguments);
Val *builtin_write (Env *environment, Val *arguments);
Val *builtin_display (Env *environment, Val *arguments);

#endif // STDIO_H_
