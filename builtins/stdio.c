#include "builtins/stdio.h"

#include <stdio.h>

#include "core/ast.h"
#include "core/eval.h"
#include "core/lexer.h"
#include "core/parser.h"

AST *
builtin_dump (AST *environment, AST *arguments)
{
  while (arguments->type == AST_CONS)
    {
      AST *value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (value);

      ast_print (value);
      printf (" ");

      arguments = CDR (arguments);
    }

  printf ("\n");
  return nil ();
}

AST *
builtin_read (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("read: expects exactly one argument");

  AST *code = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (code);
  if (code->type != AST_STRING)
    return make_error ("read: argument is not string");

  Lexer lexer = lexer_from_string (code->as.STRING, strlen (code->as.STRING));
  Parser *parser = parser_init (&lexer);
  AST *expression = parser_parse (parser);

  return expression;
}

AST *
builtin_read_file (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("read-file: expects exactly one argument");

  AST *filename = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (filename);
  if (filename->type != AST_STRING)
    return make_error ("read-file: argument is not string");

  FILE *f = fopen (filename->as.STRING, "r");
  if (!f)
    return make_error ("read-file: could not find file");

  fseek (f, 0, SEEK_END);
  long size = ftell (f);
  rewind (f);

  char *buffer = malloc (size + 1);
  fread (buffer, 1, size, f);
  buffer[size] = '\0';

  fclose (f);

  Lexer lexer = lexer_from_file (filename->as.STRING, buffer, size);
  Parser *parser = parser_init (&lexer);
  AST *expression = parser_parse (parser);

  return expression;
}

AST *
builtin_file_to_string (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("file->string: expects exactly one argument");

  AST *filename = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (filename);
  if (filename->type != AST_STRING)
    return make_error ("file->string: argument is not string");

  FILE *f = fopen (filename->as.STRING, "r");
  if (!f)
    return make_error ("file->string: could not find file");

  fseek (f, 0, SEEK_END);
  long size = ftell (f);
  rewind (f);

  char *buffer = malloc (size + 1);
  fread (buffer, 1, size, f);
  buffer[size] = '\0';

  fclose (f);

  return make_string (buffer);
}

AST *
builtin_write (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("write: expects exactly one argument");

  AST *expr = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (expr);

  char *str = ast_to_string (expr);
  if (!str)
    return make_error ("write: memory allocation failed");

  AST *result = make_string (str);
  free (str);

  return result;
}

static void display_value (AST *value);

AST *
builtin_display (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("display: expects exactly 1 argument");

  AST *value = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (value);

  display_value (value);

  return nil ();
}

static void
display_value (AST *value)
{
  switch (value->type)
    {
    case AST_NIL:
      printf ("nil");
      break;
    case AST_INTEGER:
      printf ("%ld", value->as.INTEGER);
      break;
    case AST_FLOAT:
      printf ("%g", value->as.FLOAT);
      break;
    case AST_STRING:
      printf ("%s", value->as.STRING);
      break;
    case AST_SYMBOL:
      printf ("%s", value->as.SYMBOL);
      break;

    case AST_CONS:
      {
        printf ("(");
        AST *cur = value;
        while (cur->type == AST_CONS)
          {
            display_value (CAR (cur));
            cur = CDR (cur);
            if (cur->type == AST_CONS)
              printf (" ");
          }
        if (cur->type != AST_NIL)
          {
            printf (" . ");
            display_value (cur);
          }
        printf (")");
      }
      break;

    case AST_LAMBDA:
      printf ("<lambda>");
      break;
    case AST_BUILTIN_NORMAL:
    case AST_BUILTIN_SPECIAL:
      printf ("<builtin>");
      break;

    default:
      printf ("<unknown>");
      break;
    }
}
