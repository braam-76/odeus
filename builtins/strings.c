#include "builtins/strings.h"

#include <string.h>

#include "core/eval.h"
#include "core/value.h"

Value *
builtin_concat (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) < 2)
    return val_error ("concat: expects at levalue two arguments");

  Value *evaluated = val_nil ();
  Value *tail = val_nil ();

  size_t total_length = 0;

  while (arguments->type == VALUE_CONS)
    {
      Value *value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (value);

      if (value->type != VALUE_STRING)
        return val_error ("concat: all arguments must be strings");

      total_length += strlen (value->as.STRING);

      Value *cell = val_cons (value, val_nil ());
      if (IS_NULL (evaluated))
        evaluated = tail = cell;
      else
        tail = tail->as.CONS.CDR = cell;

      arguments = CDR (arguments);
    }

  char *buffer = malloc (total_length + 1);
  if (!buffer)
    return val_error ("concat: memory allocation failed");

  char *dst = buffer;
  Value *current = evaluated;

  while (current->type == VALUE_CONS)
    {
      const char *src = CAR (current)->as.STRING;
      size_t len = strlen (src);
      memcpy (dst, src, len);
      dst += len;
      current = CDR (current);
    }

  *dst = '\0';

  Value *result = val_string (buffer);
  free (buffer);

  return result;
}

Value *
builtin_string_length (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("string-length: expects exactly one argument");

  Value *string = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (string);

  if (string->type != VALUE_STRING)
    return val_error ("string-length: argument is not string");

  return val_integer (strlen (string->as.STRING));
}

Value *
builtin_substring (Environment *environment, Value *arguments)
{
  int arguments_count = arguments_length (arguments);
  if (arguments_count < 2 || arguments_count > 3)
    return val_error (
        "substring: expects either 2 or 3 arguments: (substring [str] [low "
        "index] <high index>)");

  Value *string = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (string);
  if (string->type != VALUE_STRING)
    return val_error ("substring: first argument must be a string");

  Value *low_index = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (low_index);
  if (low_index->type != VALUE_INTEGER)
    return val_error ("substring: low index must be an integer");

  Value *high_index;
  if (arguments_count == 3)
    {
      high_index = evaluate_expression (environment, CADR (CDR (arguments)));
      if (high_index->type != VALUE_INTEGER)
        return val_error ("substring: high index must be an integer");
    }
  else
    high_index = val_integer (strlen (string->as.STRING));
  ERROR_OUT (high_index);

  const char *str = string->as.STRING;
  int len = strlen (str);
  int low = low_index->as.INTEGER;
  int high = high_index->as.INTEGER;

  // Handle negative indices (Python-style from the end)
  if (low < 0)
    low = len + low;
  if (high < 0)
    high = len + high;

  // Bound checking and adjustment
  if (low < 0)
    low = 0;
  else if (low > len)
    low = len;

  if (high < 0)
    high = len;
  else if (high > len)
    high = len;

  // Validate that low <= high
  if (low > high)
    return val_error (
        "substring: low index must be less than or equal to high index");

  // Calculate substring length
  int substr_len = high - low;

  if (substr_len <= 0)
    return val_string ("");

  char *buffer = malloc (substr_len + 1);
  if (!buffer)
    return val_error ("substring: memory allocation failed");

  memcpy (buffer, str + low, substr_len);
  buffer[substr_len] = '\0';

  Value *result = val_string (buffer);
  free (buffer);

  return result;
}

Value *
builtin_string_to_symbol (Environment *environment, Value *arguments)
{
  (void)environment;
  (void)arguments;

  // I yet dont know how to turn string to symbol to be
  // - printable
  // - usable
  // and I dont know if I should allow unallowed for symbols characters
  // and how represent all of that
  return val_error ("string->symbol: not implemented yet");
}

Value *
builtin_symbol_to_string (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("symbol->string: expects exactly one argument");

  Value *symbol_arg = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (symbol_arg);

  if (symbol_arg->type != VALUE_SYMBOL)
    return val_error ("symbol->string: argument must be a symbol");

  return val_string (symbol_arg->as.SYMBOL);
}
