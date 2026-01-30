#ifndef AST_H_
#define AST_H_

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
  AST_END_OF_FILE,
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
};

#define CAR(cons) ((cons)->as.CONS.CAR)
#define CDR(cons) ((cons)->as.CONS.CDR)

AST *ast_integer (long astue);
AST *ast_float (double astue);
AST *ast_string (const char *string);
AST *ast_symbol (const char *symbol);
AST *ast_cons (AST *car, AST *cdr);

// special ASTUE node builder, only for error messages
AST *ast_error (const char *message);

AST *nil (void);
AST *t (void);

void ast_print (AST *node);

#endif // AST_H_
