#ifndef ODEUS_PARSER_H_
#define ODEUS_PARSER_H_

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "odeus_lexer.h"
#include "odeus_token.h"

typedef enum
{
  AST_NIL,
  AST_SYMBOL,
  AST_INTEGER,
  AST_FLOAT,
  AST_STRING,
  AST_QUOTE,
  AST_CONS,

  AST_END_OF_FILE,
} AST_Node_Type;

typedef struct AST
{
  AST_Node_Type type;
  union
  {
    long INTEGER;
    double FLOAT;
    char *STRING;
    char *SYMBOL;

    struct
    {
      struct AST *EXPR;
    } QUOTE;

    struct
    {
      struct AST *CAR;
      struct AST *CDR;
    } CONS;
  } value;
  size_t line;
  size_t column;
} AST_Node;

typedef struct
{
  Lexer *lexer;
  AST_Node *start_node;
} Parser;

Parser *parser_init (Lexer *lexer);
void parser_parse (Parser *parse);
void parser_panic (Token *token, const char *filename, const char *message);
AST_Node* nil(void);

void ast_print(AST_Node *node);

#endif // ODEUS_PARSER_H_
