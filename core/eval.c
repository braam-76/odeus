#include "core/eval.h"

static AST *bind_arguments (AST *frame, AST *caller_environment, AST *parameters, AST *arguments);

AST *
evaluate_expression (AST *environment, AST *expression)
{
  if (!expression)
    return nil ();

  switch (expression->type)
    {
    case AST_INTEGER:
    case AST_FLOAT:
    case AST_STRING:
    case AST_NIL: return expression;
    case AST_QUOTE: return expression->as.QUOTE.EXPR;
    case AST_SYMBOL: return environment_get (environment, expression);
    case AST_CONS:
      {
        AST *operator_node = CAR (expression);
        AST *arguments = CDR (expression);

        /* Evaluate operator */
        AST *function = (operator_node->type == AST_SYMBOL)
                            ? environment_get (environment, operator_node)
                            : evaluate_expression (environment, operator_node);

        ERROR_OUT (function);

        return apply (function, environment, arguments);
      }

    case AST_END_OF_FILE:
    case AST_ERROR: return expression;

    default: return make_error ("evaluate_expression: unknown AST type");
    }
}

int
arguments_length (AST *arguments)
{
  int count = 0;

  while (arguments->type == AST_CONS)
    {
      count++;
      arguments = CDR (arguments);
    }

  return count;
}

static AST *
bind_arguments (AST *frame, AST *caller_environment, AST *parameters, AST *arguments)
{
  AST *params = parameters;
  AST *args = arguments;

  /* Bind fixed parameters */
  while (params->type == AST_CONS)
    {
      if (args->type != AST_CONS)
        return make_error ("lambda: too few arguments");

      AST *param = CAR (params);
      if (param->type != AST_SYMBOL)
        return make_error ("lambda parameter must be symbol");

      AST *value = evaluate_expression (caller_environment, CAR (args));
      ERROR_OUT (value);

      environment_set (frame, param, value);

      params = CDR (params);
      args = CDR (args);
    }

  /* params is now either NIL or a SYMBOL (rest parameter) */

  if (params->type == AST_SYMBOL)
    {
      /* rest parameter */
      environment_set (frame, params, args);
      return nil ();
    }

  if (params->type != AST_NIL)
    return make_error ("lambda: invalid parameter list");

  /* No rest param: ensure no extra arguments */
  if (args->type != AST_NIL)
    return make_error ("lambda: too many arguments");

  return nil ();
}

AST *
apply (AST *function, AST *environment, AST *arguments)
{
  /* Special forms */
  if (function->type == AST_BUILTIN_SPECIAL)
    {
      AST *result = function->as.BUILTIN (environment, arguments);
      ERROR_OUT (result);
      return result;
    }

  /* Normal builtins */
  if (function->type == AST_BUILTIN_NORMAL)
    return function->as.BUILTIN (environment, arguments);

  /* Lambda */
  if (function->type == AST_LAMBDA)
    {
      AST *frame = make_cons (function->as.LAMBDA.environment, nil ());

      AST *err = bind_arguments (frame, environment, function->as.LAMBDA.parameters, arguments);
      ERROR_OUT (err);

      AST *result = nil ();
      AST *body = function->as.LAMBDA.body;

      while (body->type == AST_CONS)
        {
          result = evaluate_expression (frame, CAR (body));
          ERROR_OUT (result);
          body = CDR (body);
        }

      return result;
    }

  return make_error ("attempt to call non-function");
}
