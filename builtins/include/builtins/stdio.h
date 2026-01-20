#ifndef STDIO_H_
#define STDIO_H_

#include <stdio.h>

#include "core/ast.h"
#include "core/eval.h"
#include "core/lexer.h"
#include "core/parser.h"

AST *builtin_dump (AST *environment, AST *arguments);
AST *builtin_read (AST *environment, AST *arguments);
AST *builtin_read_file (AST *environment, AST *arguments);
AST *builtin_file_to_string (AST *environment, AST *arguments);
AST *builtin_write (AST *environment, AST *arguments);
AST *builtin_display (AST *environment, AST *arguments);

#endif // STDIO_H_
