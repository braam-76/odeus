#ifndef LEXER_H_
#define LEXER_H_

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/core_error.h"
#include "core/token.h"

typedef struct
{
  char *filename;
  char *source;
  size_t source_size;
  size_t position;
  size_t line;
  size_t column;
  size_t token_start_column;

  Error error;
} Lexer;

Lexer lexer_from_file (char *filename, char *source, size_t source_size);
Lexer lexer_from_string (char *source, size_t source_size);
Token lexer_next_token (Lexer *lexer);

#endif // LEXER_H_
