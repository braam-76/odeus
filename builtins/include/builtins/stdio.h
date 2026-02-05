#ifndef STDIO_H_
#define STDIO_H_

#include <stdio.h>

#include "core/value.h"
#include "core/eval.h"
#include "core/lexer.h"
#include "core/parser.h"

Value *builtin_dump (Environment *environment, Value *arguments);
Value *builtin_read (Environment *environment, Value *arguments);
Value *builtin_read_file (Environment *environment, Value *arguments);
Value *builtin_load_file (Environment *environment, Value *arguments);
Value *builtin_show_meta (Environment *environment, Value *arguments);
Value *builtin_file_to_string (Environment *environment, Value *arguments);
Value *builtin_write (Environment *environment, Value *arguments);
Value *builtin_display (Environment *environment, Value *arguments);

#endif // STDIO_H_
