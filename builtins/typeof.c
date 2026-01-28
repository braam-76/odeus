#include "builtins/typeof.h"
#include "core/ast.h"
#include "core/eval.h"

AST *
builtin_typeof (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("typeof: expects exactly one argument\n");

  AST *expression = evaluate_expression (environment, CAR (arguments));
  switch (expression->type)
    {
    case AST_NIL:
      return make_symbol ("cons");
    case AST_SYMBOL:
      return make_symbol ("symbol");
    case AST_INTEGER:
      return make_symbol ("integer");
    case AST_FLOAT:
      return make_symbol ("float");
    case AST_STRING:
      return make_symbol ("string");
    case AST_CONS:
      return make_symbol ("cons");
    case AST_BUILTIN_NORMAL:
    case AST_BUILTIN_SPECIAL:
    case AST_LAMBDA:
      return make_symbol ("function");
    case AST_MACRO:
      return make_symbol ("macro");
    case AST_ERROR:
      return expression;
    case AST_END_OF_FILE:
      return make_error ("typeof: Unexpected end of file\n");
    default:
      return make_error ("typeof: unreachable\n");
    }
}
