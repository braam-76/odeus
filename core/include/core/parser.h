#ifndef PARSER_H_
#define PARSER_H_

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/ast.h"
#include "core/lexer.h"
#include "core/token.h"

typedef struct
{
  Lexer *lexer;
  AST *start_node;
  Error error;
} Parser;

Parser *parser_init (Lexer *lexer);
AST* parser_parse (Parser *parser);
void parser_panic (Parser *parser, Token *token, const char *message);

#endif // PARSER_H_
