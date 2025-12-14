#include "odeus_parser.h"
#include <assert.h>

AST *
make_integer (long value)
{
  AST *node = (AST *)malloc (sizeof (AST));
  node->type = AST_INTEGER;
  node->as.INTEGER = value;
  node->line = 0;
  node->column = 0;
  return node;
}

AST *
make_float (double value)
{
  AST *node = (AST *)malloc (sizeof (AST));
  node->type = AST_FLOAT;
  node->as.FLOAT = value;
  node->line = 0;
  node->column = 0;
  return node;
}

AST *
make_string (const char *string)
{
  AST *node = (AST *)malloc (sizeof (AST));
  node->type = AST_STRING;
  node->as.STRING = strdup (string);
  node->line = 0;
  node->column = 0;
  return node;
}

AST *
make_symbol (const char *symbol)
{
  AST *node = (AST *)malloc (sizeof (AST));
  node->type = AST_SYMBOL;
  node->as.SYMBOL = strdup (symbol);
  node->line = 0;
  node->column = 0;
  return node;
}

AST *
make_builtin (Builtin_Function builtin_function, AST_Type kind)
{
  assert (kind == AST_BUILTIN_NORMAL || kind == AST_BUILTIN_SPECIAL);

  AST *node = (AST *)malloc (sizeof (AST));
  node->type = kind;
  node->as.BUILTIN = builtin_function;
  node->line = 0;
  node->column = 0;
  return node;
}

AST *
make_error (const char *message)
{
  AST *node = (AST *)malloc (sizeof (AST));
  node->type = AST_ERROR;
  node->as.ERROR.MESSAGE = strdup (message);
  node->line = 0;
  node->column = 0;
  return node;
}
