#include "builtins/stdio.h"

#include <stdio.h>

#include "builtins/forms.h"
#include "core/ast.h"
#include "core/environment.h"
#include "core/eval.h"
#include "core/value.h"

Value *
builtin_dump (Environment *environment, Value *arguments)
{
  while (arguments->type == VALUE_CONS)
    {
      Value *value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (value);

      value_print (value);
      printf (" ");

      arguments = CDR (arguments);
    }

  printf ("\n");
  return val_nil ();
}

Value *
builtin_read (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("read: expects exactly one argument");

  Value *code = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (code);
  if (code->type != VALUE_STRING)
    return val_error ("read: argument is not string");

  Lexer lexer = lexer_from_string (code->as.STRING, strlen (code->as.STRING));
  Parser *parser = parser_init (&lexer);
  AST *expression = parser_parse (parser);
  Value *lower = val_from_ast (expression);

  return lower;
}

Value *
builtin_read_file (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("read-file: expects exactly one argument");

  Value *filename = evaluate_expression (environment, CAR (arguments));
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
  Value *lower = val_from_ast (expression);

  free(buffer);

  return lower;
}

Value *
builtin_load_file (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("load-file: expects exactly one argument");

  Value *filename = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (filename);

  if (filename->type != VALUE_STRING)
    return val_error ("load-file: filename must be a string");

  Value *read_args = val_cons (filename, val_nil ());
  Value *program = builtin_read_file (environment, read_args);

  if (program->type == VALUE_ERROR)
    {
      char error_msg[256];
      snprintf (error_msg, sizeof (error_msg),
                "load-file: error reading %s: %s", filename->as.STRING,
                program->as.ERROR.MESSAGE);
      return val_error (error_msg);
    }

  Value *eval_args = val_cons (program, val_nil ());
  Value *result = builtin_eval (environment, eval_args);

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

Value *
builtin_reload_file (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("reload-file: expects exactly one argument");

  Value *value = CAR (arguments);
  if (value->type != VALUE_STRING)
    return val_error ("reload-file: argument is not string");

  char *val_str = value->as.STRING;

  for (int i = 0; i < environment->bindings_size; i++)
    {

      Binding binding = environment->bindings[i];
      if (binding.meta.filename
          && strcmp (binding.meta.filename, val_str) == 0)
        {
          for (int j = i; j < environment->bindings_size - 1; j++)
            environment->bindings[j] = environment->bindings[j + 1];
          environment->bindings_size--;
          i--;
        }
    }

  return builtin_load_file (environment, arguments);
}

Value *
builtin_show_meta (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("show-meta: expects exactly one argument");

  Value *symbol = CAR (arguments);
  if (symbol->type != VALUE_SYMBOL)
    return val_error ("show-meta: argument is not symbol");

  Binding *binding = env_get_binding (environment, symbol);
  if (!binding)
    return val_error ("show-meta: symbol is not defined");

  printf ("Filename: %s\n", binding->meta.filename);
  printf ("Line: %d\n", binding->meta.line_number);

  return val_nil ();
}

Value *
builtin_file_to_string (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("file->string: expects exactly one argument");

  Value *filename = evaluate_expression (environment, CAR (arguments));
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

  Value* result =val_string (buffer);

  free(buffer);
  return result;
}

Value *
builtin_write (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("write: expects exactly one argument");

  Value *expr = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (expr);

  char *str = value_to_string (expr);
  if (!str)
    return val_error ("write: memory allocation failed");

  Value *result = val_string (str);
  free (str);

  return result;
}

static void display_value (Value *value);

Value *
builtin_display (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("display: expects exactly 1 argument");

  Value *value = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (value);

  display_value (value);

  return val_nil ();
}

static void
display_value (Value *value)
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
        Value *cur = value;
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
