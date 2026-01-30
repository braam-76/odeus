#include "core/value.h"

#include <stdarg.h> // for make_error

static Val *GLOBAL_NIL = NULL;

// symbol table used for interning symbols
#define MAX_SYMBOLS 4096
static Val *symbol_table[MAX_SYMBOLS];
static int symbol_count = 0;

Val *
nil (void)
{
  if (!GLOBAL_NIL)
    {
      GLOBAL_NIL = malloc (sizeof (Val));
      GLOBAL_NIL->type = VALUE_NIL;
    }
  return GLOBAL_NIL;
}

Val *
t (void)
{
  return make_symbol ("t");
}

Val *
make_integer (long value)
{
  Val *node = (Val *)malloc (sizeof (Val));
  node->type = VALUE_INTEGER;
  node->as.INTEGER = value;
  return node;
}

Val *
make_float (double value)
{
  Val *node = (Val *)malloc (sizeof (Val));
  node->type = VALUE_FLOAT;
  node->as.FLOAT = value;
  return node;
}

Val *
make_string (const char *string)
{
  Val *node = (Val *)malloc (sizeof (Val));
  node->type = VALUE_STRING;
  node->as.STRING = strdup (string);
  return node;
}

Val *
make_cons (Val *car, Val *cdr)
{
  Val *n = malloc (sizeof (Val));
  n->type = VALUE_CONS;
  CAR (n) = car;
  CDR (n) = cdr;
  return n;
}

Val *
make_builtin (Builtin_Function builtin_function)
{
  Val *node = (Val *)malloc (sizeof (Val));
  node->type = VALUE_BUILTIN;
  node->as.BUILTIN = builtin_function;
  return node;
}

Val *
make_error (const char *message)
{
  Val *node = (Val *)malloc (sizeof (Val));
  node->type = VALUE_ERROR;
  node->as.ERROR.MESSAGE = strdup (message);
  return node;
}

Val *
make_symbol (const char *symbol)
{
  for (int i = 0; i < symbol_count; i++)
    if (strcmp (symbol_table[i]->as.SYMBOL, symbol) == 0)
      return symbol_table[i];

  if (symbol_count >= MAX_SYMBOLS)
    return make_error (
        "FATAL ERROR: can't create more symbols. symbol_table is full");

  Val *sym = malloc (sizeof (Val));
  sym->type = VALUE_SYMBOL;
  sym->as.SYMBOL = strdup (symbol);

  symbol_table[symbol_count++] = sym;
  return sym;
}

void
value_print (Val *node)
{
  if (!node)
    {
      printf ("()");
      return;
    }

  switch (node->type)
    {
    case VALUE_NIL:
      printf ("nil");
      break;
    case VALUE_SYMBOL:
      printf ("%s", node->as.SYMBOL);
      break;
    case VALUE_INTEGER:
      printf ("%ld", node->as.INTEGER);
      break;
    case VALUE_FLOAT:
      printf ("%g", node->as.FLOAT);
      break;

    case VALUE_STRING:
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

    case VALUE_CONS:
      {
        printf ("(");
        Val *cur = node;

        while (cur->type == VALUE_CONS)
          {
            value_print (CAR (cur));
            cur = CDR (cur);

            if (cur->type == VALUE_CONS)
              printf (" ");
          }

        if (cur->type != VALUE_NIL)
          {
            printf (" . ");
            value_print (cur);
          }
        printf (")");
        break;
      }

    case VALUE_BUILTIN:
      printf ("#<builtin function>");
      break;

    case VALUE_LAMBDA:
      printf ("#<lambda>");
      break;
    case VALUE_MACRO:
      printf ("#<macro>");
      break;

    case VALUE_ERROR:
      printf ("%s", node->as.ERROR.MESSAGE);
      break;
    case VALUE_END_OF_FILE:
      printf ("#<EOF>");
      break;

    default:
      printf ("#<UNKNOWN:%d>", node->type);
      break;
    }
}
