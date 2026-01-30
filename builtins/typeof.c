#include "builtins/typeof.h"
#include "core/value.h"
#include "core/eval.h"

Val *
builtin_typeof (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("typeof: expects exactly one argument\n");

  Val *expression = evaluate_expression (environment, CAR (arguments));
  switch (expression->type)
    {
    case VALUE_NIL:
      return val_symbol ("cons");
    case VALUE_SYMBOL:
      return val_symbol ("symbol");
    case VALUE_INTEGER:
      return val_symbol ("integer");
    case VALUE_FLOAT:
      return val_symbol ("float");
    case VALUE_STRING:
      return val_symbol ("string");
    case VALUE_CONS:
      return val_symbol ("cons");
    case VALUE_BUILTIN:
    case VALUE_LAMBDA:
      return val_symbol ("function");
    case VALUE_MACRO:
      return val_symbol ("macro");
    case VALUE_ERROR:
      return expression;
    case VALUE_END_OF_FILE:
      return val_error ("typeof: Unexpected end of file\n");
    default:
      return val_error ("typeof: unreachable\n");
    }
}
