#ifndef ODEUS_PARSER_H_
#define ODEUS_PARSER_H_

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "odeus_ast.h"
#include "odeus_lexer.h"
#include "odeus_token.h"

typedef struct
{
  Lexer *lexer;
  AST *start_node;
  Error error;
} Parser;

Parser *parser_init (Lexer *lexer);
void parser_parse (Parser *parser);
void parser_panic (Parser *parser, Token *token, const char *message);

#endif // ODEUS_PARSER_H_
