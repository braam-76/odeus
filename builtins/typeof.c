#include "builtins/typeof.h"
#include "core/value.h"
#include "core/eval.h"

Value *
builtin_typeof (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("typeof: expects exactly one argument\n");

  Value *expression = evaluate_expression (environment, CAR (arguments));
  switch (expression->type)
    {
    case VALUE_NIL:
      return val_symbol ("nil", expression->meta);
    case VALUE_SYMBOL:
      return val_symbol ("symbol", expression->meta);
    case VALUE_INTEGER:
      return val_symbol ("integer", expression->meta);
    case VALUE_FLOAT:
      return val_symbol ("float", expression->meta);
    case VALUE_STRING:
      return val_symbol ("string", expression->meta);
    case VALUE_CONS:
      return val_symbol ("cons", expression->meta);
    case VALUE_BUILTIN:
    case VALUE_LAMBDA:
      return val_symbol ("function", expression->meta);
    case VALUE_MACRO:
      return val_symbol ("macro", expression->meta);
    case VALUE_ERROR:
      return expression;
    case VALUE_END_OF_FILE:
      return val_error ("typeof: Unexpected end of file\n");
    default:
      return val_error ("typeof: unreachable\n");
    }
}
