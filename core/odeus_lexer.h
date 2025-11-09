#ifndef ODEUS_LEXER_H_
#define ODEUS_LEXER_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "odeus_token.h"

typedef struct
{
    const char* filename;
    const char* source;
    size_t      source_size;
    size_t      position;
    size_t      line;
    size_t      column;
} core_Lexer;

core_Lexer core_lexer_init       (const char* filename, const char* source, size_t source_size);
core_Token core_lexer_next_token (core_Lexer* lexer);

#endif // ODEUS_LEXER_H_
