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

  AST_BUILTIN_NORMAL,
  AST_BUILTIN_SPECIAL,
  AST_LAMBDA,

  AST_ERROR,
  AST_END_OF_FILE,
} AST_Type;

typedef struct AST_Node AST;
typedef AST *(*Builtin_Function) (AST *environment, AST *arguments);

struct AST_Node
{
  AST_Type type;
  union
  {
    long INTEGER;
    double FLOAT;
    char *STRING;
    char *SYMBOL;

    struct
    {
      AST *EXPR;
    } QUOTE;

    struct
    {
      AST *CAR;
      AST *CDR;
    } CONS;

    struct
    {
      char *MESSAGE;
    } ERROR;

    // used in eval step
    Builtin_Function BUILTIN;
    struct
    {
      AST *environment;
      AST *parameters;
      AST *body;
    } LAMBDA;
  } as;
  size_t line;
  size_t column;
};

typedef struct
{
  Lexer *lexer;
  AST *start_node;
  Error error;
} Parser;

Parser *parser_init (Lexer *lexer);
void parser_parse (Parser *parser);
void parser_panic (Parser *parser, Token *token, const char *message);

AST *make_integer (long value);
AST *make_float (double value);
AST *make_string (const char *string);
AST *make_symbol (const char *symbol);
AST *make_cons (AST *car, AST *cdr);

// 'kind' should be either AST_BUILTIN_NORMAL or AST_BUILTIN_SPECIAL
AST *make_builtin (Builtin_Function builtin_function, AST_Type kind);

// special AST node builder, only for error messages
AST* make_error (const char* message);

AST *nil (void);
AST *t (void);

#define CAR(cons) ((cons)->as.CONS.CAR)
#define CDR(cons) ((cons)->as.CONS.CDR)

#define CAAR(cons) (CAR (CAR ((cons))))
#define CADR(cons) (CAR (CDR ((cons))))
#define CDAR(cons) (CDR (CAR ((cons))))
#define CDDR(cons) (CDR (CDR ((cons))))

#define IS_NULL(a) ((a) == NULL || (a)->type == AST_NIL)

void ast_print (AST *node);

#endif // ODEUS_PARSER_H_
