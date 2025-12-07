#ifndef ODEUS_LEXER_H_
#define ODEUS_LEXER_H_

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "odeus_token.h"

typedef struct
{
  char *filename;
  char *source;
  size_t source_size;
  size_t position;
  size_t line;
  size_t column;
  size_t token_start_column; // special field to fix tokens column value problem
} Lexer;

Lexer lexer_from_file (char *filename, char *source, size_t source_size);
Lexer lexer_from_string (char* source, size_t source_size);
Token lexer_next_token (Lexer *lexer);
void lexer_free (Lexer *lexer);

#endif // ODEUS_LEXER_H_
