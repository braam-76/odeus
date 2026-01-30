#ifndef STDIO_H_
#define STDIO_H_

#include <stdio.h>

#include "core/value.h"
#include "core/eval.h"
#include "core/lexer.h"
#include "core/parser.h"

Val *builtin_dump (Val *environment, Val *arguments);
Val *builtin_read (Val *environment, Val *arguments);
Val *builtin_read_file (Val *environment, Val *arguments);
Val *builtin_load_file (Val *environment, Val *arguments);
Val *builtin_file_to_string (Val *environment, Val *arguments);
Val *builtin_write (Val *environment, Val *arguments);
Val *builtin_display (Val *environment, Val *arguments);

#endif // STDIO_H_
