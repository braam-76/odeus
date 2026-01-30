#include "builtins/strings.h"

#include <string.h>

#include "core/value.h"
#include "core/eval.h"

Val *
builtin_concat (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) < 2)
    return make_error ("concat: expects at levalue two arguments");

  Val *evaluated = nil ();
  Val *tail = nil ();

  size_t total_length = 0;

  while (arguments->type == VALUE_CONS)
    {
      Val *value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (value);

      if (value->type != VALUE_STRING)
        return make_error ("concat: all arguments must be strings");

      total_length += strlen (value->as.STRING);

      Val *cell = make_cons (value, nil ());
      if (IS_NULL (evaluated))
        evaluated = tail = cell;
      else
        tail = tail->as.CONS.CDR = cell;

      arguments = CDR (arguments);
    }

  char *buffer = malloc (total_length + 1);
  if (!buffer)
    return make_error ("concat: memory allocation failed");

  char *dst = buffer;
  Val *current = evaluated;

  while (current->type == VALUE_CONS)
    {
      const char *src = CAR (current)->as.STRING;
      size_t len = strlen (src);
      memcpy (dst, src, len);
      dst += len;
      current = CDR (current);
    }

  *dst = '\0';

  Val *result = make_string (buffer);
  free (buffer);

  return result;
}

Val *
builtin_string_length (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("string-length: expects exactly one argument");

  Val *string = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (string);

  if (string->type != VALUE_STRING)
    return make_error ("string-length: argument is not string");

  return make_integer (strlen (string->as.STRING));
}

Val *
builtin_substring (Val *environment, Val *arguments)
{
  int arguments_count = arguments_length (arguments);
  if (arguments_count < 2 || arguments_count > 3)
    return make_error (
        "substring: expects either 2 or 3 arguments: (substring [str] [low "
        "index] <high index>)");

  Val *string = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (string);
  if (string->type != VALUE_STRING)
    return make_error ("substring: first argument must be a string");

  Val *low_index = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (low_index);
  if (low_index->type != VALUE_INTEGER)
    return make_error ("substring: low index must be an integer");

  Val *high_index;
  if (arguments_count == 3)
    {
      high_index = evaluate_expression (environment, CADR (CDR (arguments)));
      if (high_index->type != VALUE_INTEGER)
        return make_error ("substring: high index must be an integer");
    }
  else
    high_index = make_integer (strlen (string->as.STRING));
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
    return make_error (
        "substring: low index must be less than or equal to high index");

  // Calculate substring length
  int substr_len = high - low;

  if (substr_len <= 0)
    return make_string ("");

  char *buffer = malloc (substr_len + 1);
  if (!buffer)
    return make_error ("substring: memory allocation failed");

  memcpy (buffer, str + low, substr_len);
  buffer[substr_len] = '\0';

  Val *result = make_string (buffer);
  free (buffer);

  return result;
}

Val *
builtin_string_to_symbol (Val *environment, Val *arguments)
{
  (void)environment;
  (void)arguments;

  // I yet dont know how to turn string to symbol to be
  // - printable
  // - usable
  // and I dont know if I should allow unallowed for symbols characters
  // and how represent all of that
  return make_error ("string->symbol: not implemented yet");
}

Val *
builtin_symbol_to_string (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("symbol->string: expects exactly one argument");

  Val *symbol_arg = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (symbol_arg);

  if (symbol_arg->type != VALUE_SYMBOL)
    return make_error ("symbol->string: argument must be a symbol");

  return make_string (symbol_arg->as.SYMBOL);
}
