#ifndef AST_H_
#define AST_H_

#include "core/meta.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
  AST_NIL,
  AST_SYMBOL,
  AST_INTEGER,
  AST_FLOAT,
  AST_STRING,
  AST_CONS,

  AST_ERROR,
} ASTType;

typedef struct ASTNode AST;

struct ASTNode
{
  ASTType type;
  union
  {
    long INTEGER;
    double FLOAT;
    char *STRING;
    char *SYMBOL;

    struct
    {
      AST *CAR;
      AST *CDR;
    } CONS;

    struct
    {
      char *MESSAGE;
    } ERROR;
  } as;

  Meta source_meta;
};

#define CAR(cons) ((cons)->as.CONS.CAR)
#define CDR(cons) ((cons)->as.CONS.CDR)

AST *ast_nil ();
AST *ast_integer (long v);
AST *ast_float (double v);
AST *ast_string (const char *string);
AST* ast_symbol (const char *symbol, Meta meta);
AST *ast_cons (AST *car, AST *cdr);

// special AST node builder, only for error messages
AST *ast_error (const char *message);

void ast_print (AST *node);

#endif // AST_H_
