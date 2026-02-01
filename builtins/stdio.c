#include "builtins/stdio.h"

#include <stdio.h>

#include "builtins/forms.h"
#include "core/ast.h"
#include "core/value.h"

Val *
builtin_dump (Env *environment, Val *arguments)
{
  while (arguments->type == VALUE_CONS)
    {
      Val *value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (value);

      value_print (value);
      printf (" ");

      arguments = CDR (arguments);
    }

  printf ("\n");
  return val_nil ();
}

Val *
builtin_read (Env *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("read: expects exactly one argument");

  Val *code = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (code);
  if (code->type != VALUE_STRING)
    return val_error ("read: argument is not string");

  Lexer lexer = lexer_from_string (code->as.STRING, strlen (code->as.STRING));
  Parser *parser = parser_init (&lexer);
  AST *expression = parser_parse (parser);
  Val *lower = val_from_ast (expression);

  return lower;
}

Val *
builtin_read_file (Env *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("read-file: expects exactly one argument");

  Val *filename = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (filename);
  if (filename->type != VALUE_STRING)
    return val_error ("read-file: argument is not string");

  FILE *f = fopen (filename->as.STRING, "r");
  if (!f)
    return val_error ("read-file: could not find file");

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
  Val *lower = val_from_ast (expression);

  return lower;
}

Val *
builtin_load_file (Env *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("load-file: expects exactly one argument");

  Val *filename = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (filename);

  if (filename->type != VALUE_STRING)
    return val_error ("load-file: filename must be a string");

  Val *read_args = val_cons (filename, val_nil ());
  Val *program = builtin_read_file (environment, read_args);

  if (program->type == VALUE_ERROR)
    {
      char error_msg[256];
      snprintf (error_msg, sizeof (error_msg),
                "load-file: error reading %s: %s", filename->as.STRING,
                program->as.ERROR.MESSAGE);
      return val_error (error_msg);
    }

  Val *eval_args = val_cons (program, val_nil ());
  Val *result = builtin_eval (environment, eval_args);

  if (result->type == VALUE_ERROR)
    {
      char error_msg[256];
      snprintf (error_msg, sizeof (error_msg),
                "load-file: error evaluating %s: %s", filename->as.STRING,
                result->as.ERROR.MESSAGE);
      return val_error (error_msg);
    }

  return result;
}

Val *
builtin_file_to_string (Env *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("file->string: expects exactly one argument");

  Val *filename = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (filename);
  if (filename->type != VALUE_STRING)
    return val_error ("file->string: argument is not string");

  FILE *f = fopen (filename->as.STRING, "r");
  if (!f)
    return val_error ("file->string: could not find file");

  fseek (f, 0, SEEK_END);
  long size = ftell (f);
  rewind (f);

  char *buffer = malloc (size + 1);
  fread (buffer, 1, size, f);
  buffer[size] = '\0';

  fclose (f);

  return val_string (buffer);
}

Val *
builtin_write (Env *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("write: expects exactly one argument");

  Val *expr = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (expr);

  char *str = value_to_string (expr);
  if (!str)
    return val_error ("write: memory allocation failed");

  Val *result = val_string (str);
  free (str);

  return result;
}

static void display_value (Val *value);

Val *
builtin_display (Env *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("display: expects exactly 1 argument");

  Val *value = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (value);

  display_value (value);

  return val_nil ();
}

static void
display_value (Val *value)
{
  switch (value->type)
    {
    case VALUE_NIL:
      printf ("nil");
      break;
    case VALUE_INTEGER:
      printf ("%ld", value->as.INTEGER);
      break;
    case VALUE_FLOAT:
      printf ("%g", value->as.FLOAT);
      break;
    case VALUE_STRING:
      printf ("%s", value->as.STRING);
      break;
    case VALUE_SYMBOL:
      printf ("%s", value->as.SYMBOL);
      break;

    case VALUE_CONS:
      {
        printf ("(");
        Val *cur = value;
        while (cur->type == VALUE_CONS)
          {
            display_value (CAR (cur));
            cur = CDR (cur);
            if (cur->type == VALUE_CONS)
              printf (" ");
          }
        if (cur->type != VALUE_NIL)
          {
            printf (" . ");
            display_value (cur);
          }
        printf (")");
      }
      break;

    case VALUE_LAMBDA:
      printf ("<lambda>");
      break;
    case VALUE_BUILTIN:
      printf ("<builtin>");
      break;

    default:
      printf ("<unknown>");
      break;
    }
}
