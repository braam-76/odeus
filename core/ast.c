#include "ast.h"

static AST *GLOBAL_NIL = NULL;
static AST *GLOBAL_T = NULL;

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
    {
      GLOBAL_T = malloc (sizeof (AST));
      GLOBAL_T->type = AST_SYMBOL;
      GLOBAL_T->as.SYMBOL = "t";
      GLOBAL_T->line = 0;
      GLOBAL_T->column = 0;
    }
  return GLOBAL_T;
}

/* ----------------------------------------------------------
 *   PRINTING
 * ---------------------------------------------------------- */

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
    case AST_NIL: printf ("nil"); break;

    case AST_SYMBOL: printf ("%s", node->as.SYMBOL); break;

    case AST_INTEGER: printf ("%ld", node->as.INTEGER); break;

    case AST_FLOAT: printf ("%f", node->as.FLOAT); break;

    case AST_STRING: printf ("\"%s\"", node->as.STRING); break;

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
    case AST_BUILTIN_SPECIAL: printf ("#<BUILTIN:%p>", node->as.BUILTIN); break;

    case AST_ERROR: printf ("%s", node->as.ERROR.MESSAGE); break;
    case AST_END_OF_FILE: printf ("#<EOF>"); break;

    default: printf ("#<UNKNOWN>"); break;
    }
}

void
ast_free (AST *node)
{
  if (!node)
    return;

  switch (node->type)
    {
    case AST_STRING: free (node->as.STRING); break;

    case AST_SYMBOL: free (node->as.SYMBOL); break;

    case AST_CONS:
      ast_free (CAR (node));
      ast_free (CDR (node));
      break;

    case AST_QUOTE: ast_free (node->as.QUOTE.EXPR); break;

    case AST_ERROR: free (node->as.ERROR.MESSAGE); break;

    default: break;
    }

  free (node);
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
