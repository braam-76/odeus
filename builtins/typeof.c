#include "builtins/typeof.h"
#include "core/value.h"
#include "core/eval.h"

Val *
builtin_typeof (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("typeof: expects exactly one argument\n");

  Val *expression = evaluate_expression (environment, CAR (arguments));
  switch (expression->type)
    {
    case VALUE_NIL:
      return make_symbol ("cons");
    case VALUE_SYMBOL:
      return make_symbol ("symbol");
    case VALUE_INTEGER:
      return make_symbol ("integer");
    case VALUE_FLOAT:
      return make_symbol ("float");
    case VALUE_STRING:
      return make_symbol ("string");
    case VALUE_CONS:
      return make_symbol ("cons");
    case VALUE_BUILTIN:
    case VALUE_LAMBDA:
      return make_symbol ("function");
    case VALUE_MACRO:
      return make_symbol ("macro");
    case VALUE_ERROR:
      return expression;
    case VALUE_END_OF_FILE:
      return make_error ("typeof: Unexpected end of file\n");
    default:
      return make_error ("typeof: unreachable\n");
    }
}
