#include "core/value.h"
#include "core/eval.h"
#include "core/intern_string.h"

#include <gc/gc.h>
#include <stdarg.h>

static Value *GLOBAL_NIL = NULL;

Value *
val_from_ast (AST *node)
{
  switch (node->type)
    {
    case AST_NIL:
      return val_nil ();
    case AST_SYMBOL:
      return val_symbol (node->as.SYMBOL, node->source_meta);
    case AST_INTEGER:
      return val_integer (node->as.INTEGER);
    case AST_FLOAT:
      return val_float (node->as.FLOAT);
    case AST_STRING:
      return val_string (node->as.STRING);
    case AST_CONS:
      {
        Value *car = val_from_ast (CAR (node));
        ERROR_OUT (car);
        Value *cdr = val_from_ast (CDR (node));
        ERROR_OUT (cdr);

        return val_cons (car, cdr);
      }
    case AST_ERROR:
      return val_error (node->as.ERROR.MESSAGE);
    default:
      return val_error ("ERROR: val_from_ast: Unknown ast type");
    }
}

Value *
val_nil (void)
{
  if (!GLOBAL_NIL)
    {
      GLOBAL_NIL = GC_malloc (sizeof (Value));
      memset (GLOBAL_NIL, 0, sizeof (AST));
      GLOBAL_NIL->type = VALUE_NIL;
    }
  return GLOBAL_NIL;
}

Value *
val_t (void)
{
  return val_symbol ("t", (Meta){
                              .filename = "#<core>",
                              .line_number = 0,
                          });
}

Value *
val_integer (long value)
{
  Value *node = (Value *)GC_malloc (sizeof (Value));
  memset (node, 0, sizeof (AST));
  node->type = VALUE_INTEGER;
  node->as.INTEGER = value;
  return node;
}

Value *
val_float (double value)
{
  Value *node = (Value *)GC_malloc (sizeof (Value));
  memset (node, 0, sizeof (AST));
  node->type = VALUE_FLOAT;
  node->as.FLOAT = value;
  return node;
}

Value *
val_string (const char *string)
{
  Value *node = (Value *)GC_malloc (sizeof (Value));
  memset (node, 0, sizeof (AST));
  node->type = VALUE_STRING;
  node->as.STRING = GC_strdup (string);
  return node;
}

Value *
val_cons (Value *car, Value *cdr)
{
  Value *node = GC_malloc (sizeof (Value));
  memset (node, 0, sizeof (AST));
  node->type = VALUE_CONS;
  CAR (node) = car;
  CDR (node) = cdr;
  return node;
}

Value *
val_builtin (Builtin_Function builtin_function)
{
  Value *node = (Value *)GC_malloc (sizeof (Value));
  memset (node, 0, sizeof (AST));
  node->type = VALUE_BUILTIN;
  node->as.BUILTIN = builtin_function;
  return node;
}

Value *
val_symbol (const char *symbol, Meta meta)
{
  const char *interned_name = intern_string (symbol);

  Value *node = GC_malloc (sizeof (Value));
  memset (node, 0, sizeof (AST));
  node->type = VALUE_SYMBOL;
  node->as.SYMBOL = (char *)interned_name;
  node->meta = meta;

  return node;
}

Value *
val_error (const char *format, ...)
{
  va_list arguments;

  va_start (arguments, format);
  int length = vsnprintf (NULL, 0, format, arguments);
  va_end (arguments);

  if (length < 0)
    {
      fprintf (stderr,
               "INTERNAL-VALUE-ERROR: could not compute error string length");
      exit (1);
    }

  char *error_msg = (char *)malloc (length + 1);
  if (!error_msg)
    {
      fprintf (stderr, "INTERNAL-VALUE-ERROR: memory allocation failed");
      exit (1);
    }

  va_start (arguments, format);
  vsnprintf (error_msg, length + 1, format, arguments);
  va_end (arguments);

  Value *node = (Value *)GC_malloc (sizeof (Value));
  memset (node, 0, sizeof (AST));
  if (!node)
    {
      free (error_msg);
      fprintf (stderr, "INTERNAL-VAL-ERROR: memory allocation failed");
      exit (1);
    }

  node->type = VALUE_ERROR;
  node->as.ERROR.MESSAGE = error_msg;

  return node;
}

void
value_print (Value *node)
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
        Value *cur = node;

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
      printf ("value: #<UNKNOWN:%d>", node->type);
      break;
    }
}
