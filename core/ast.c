#include "core/ast.h"

static AST *GLOBAL_NIL = NULL;
static AST *GLOBAL_T = NULL;

// symbol table used for interning symbols
#define MAX_SYMBOLS 4096
static AST *symbol_table[MAX_SYMBOLS];
static int symbol_count = 0;

AST *
nil (void)
{
  if (!GLOBAL_NIL)
    {
      GLOBAL_NIL = malloc (sizeof (AST));
      GLOBAL_NIL->type = AST_NIL;
      GLOBAL_NIL->line = 0;
      GLOBAL_NIL->column = 0;
    }
  return GLOBAL_NIL;
}

AST *
t (void)
{
  if (!GLOBAL_T)
    GLOBAL_T = make_symbol ("t");
  return GLOBAL_T;
}

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
make_quote (AST *expression)
{
  AST *node = (AST *)malloc (sizeof (AST));
  node->type = AST_QUOTE;
  node->as.QUOTE.EXPR = expression;
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
make_cons (AST *car, AST *cdr)
{
  AST *n = malloc (sizeof (AST));
  n->type = AST_CONS;
  CAR (n) = car;
  CDR (n) = cdr;
  n->line = car->line;
  n->column = car->column;
  return n;
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

AST *
make_symbol (const char *symbol)
{
  for (int i = 0; i < symbol_count; i++)
    if (strcmp (symbol_table[i]->as.SYMBOL, symbol) == 0)
      return symbol_table[i];

  if (symbol_count >= MAX_SYMBOLS)
    return make_error (
        "FATAL ERROR: can't create more symbols. symbol_table is full");

  AST *sym = malloc (sizeof (AST));
  sym->type = AST_SYMBOL;
  sym->as.SYMBOL = strdup (symbol);

  symbol_table[symbol_count++] = sym;
  return sym;
}

void
ast_print (AST *node)
{
  if (!node)
    {
      printf ("()");
      return;
    }

  switch (node->type)
    {
    case AST_NIL:
      printf ("nil");
      break;
    case AST_SYMBOL:
      printf ("%s", node->as.SYMBOL);
      break;
    case AST_INTEGER:
      printf ("%ld", node->as.INTEGER);
      break;
    case AST_FLOAT:
      printf ("%g", node->as.FLOAT);
      break;
    case AST_STRING:
      printf ("\"%s\"", node->as.STRING);
      break;

    case AST_QUOTE:
      printf ("'");
      ast_print (node->as.QUOTE.EXPR);
      break;

    case AST_CONS:
      {
        printf ("(");
        AST *cur = node;

        while (cur->type == AST_CONS)
          {
            ast_print (CAR (cur));
            cur = CDR (cur);

            if (cur->type == AST_CONS)
              printf (" ");
          }

        if (cur->type != AST_NIL)
          {
            printf (" . ");
            ast_print (cur);
          }
        printf (")");
        break;
      }

    case AST_BUILTIN_NORMAL:
      printf ("#<builtin function>");
      break;
    case AST_BUILTIN_SPECIAL:
      printf ("#<special form>");
      break;

    case AST_ERROR:
      printf ("%s", node->as.ERROR.MESSAGE);
      break;
    case AST_END_OF_FILE:
      printf ("#<EOF>");
      break;

    default:
      printf ("#<UNKNOWN:%d>", node->type);
      break;
    }
}
