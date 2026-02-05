#include "core/ast.h"
#include "core/intern_string.h"

#include <stdarg.h> // for make_error

static AST *GLOBAL_NIL = NULL;

AST *
ast_nil ()
{
  if (!GLOBAL_NIL)
    {
      GLOBAL_NIL = calloc (1, sizeof (AST));
      GLOBAL_NIL->type = AST_NIL;
    }
  return GLOBAL_NIL;
}

AST *
ast_integer (long ast)
{
  AST *node = (AST *)calloc (1, sizeof (AST));
  node->type = AST_INTEGER;
  node->as.INTEGER = ast;
  return node;
}

AST *
ast_float (double ast)
{
  AST *node = (AST *)calloc (1, sizeof (AST));
  node->type = AST_FLOAT;
  node->as.FLOAT = ast;
  return node;
}

AST *
ast_string (const char *string)
{
  AST *node = (AST *)calloc (1, sizeof (AST));
  node->type = AST_STRING;
  node->as.STRING = strdup (string);
  return node;
}

AST *
ast_cons (AST *car, AST *cdr)
{
  AST *n = calloc (1, sizeof (AST));
  n->type = AST_CONS;
  CAR (n) = car;
  CDR (n) = cdr;
  return n;
}

AST *
ast_error (const char *message)
{
  AST *node = (AST *)calloc (1, sizeof (AST));
  node->type = AST_ERROR;
  node->as.ERROR.MESSAGE = strdup (message);
  return node;
}

AST *
ast_symbol (const char *symbol, Meta meta)
{
  const char *interned_name = intern_string (symbol);

  AST *sym = calloc (1, sizeof (AST));
  sym->type = AST_SYMBOL;
  sym->as.SYMBOL = (char *)interned_name;
  sym->source_meta = meta;
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
      {
        char *s = node->as.STRING;
        putchar ('"');
        for (; *s; s++)
          {
            switch (*s)
              {
              case '\n':
                printf ("\\n");
                break;
              case '\t':
                printf ("\\t");
                break;
              case '\r':
                printf ("\\r");
                break;
              case '\b':
                printf ("\\b");
                break;
              case '\f':
                printf ("\\f");
                break;
              case '\\':
                printf ("\\\\");
                break;
              case '"':
                printf ("\\\"");
                break;
              default:
                if ((unsigned char)*s < 32 || (unsigned char)*s >= 127)
                  printf ("\\x%02x", (unsigned char)*s);
                else
                  putchar (*s);
              }
          }
        putchar ('"');
      }
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

    case AST_ERROR:
      printf ("%s", node->as.ERROR.MESSAGE);
      break;

    default:
      printf ("#<UNKNOWN:%d>", node->type);
      break;
    }
}
