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
  } as;
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

AST_Node *symbol (char* name);
AST_Node *cons (AST_Node *car, AST_Node *cdr);
AST_Node *nil (void);

#define CAR(cons) ((cons)->as.CONS.CAR)
#define CDR(cons) ((cons)->as.CONS.CDR)

#define CAAR(cons) (CAR (CAR ((cons))))
#define CADR(cons) (CAR (CDR ((cons))))
#define CDAR(cons) (CDR (CAR ((cons))))
#define CDDR(cons) (CDR (CDR ((cons))))

#define IS_NULL(a)((a) == NULL || (a)->type == AST_NIL)

void ast_print(AST_Node *node);

#endif // ODEUS_PARSER_H_
