#include "eval.h"

static AST *evaluate_arguments (AST *environment, AST *arguments);
AST *evaluate_expression (AST *environment, AST *expression);

static AST *
bind_arguments (AST *environment, AST *params, AST *args)
{
  while (params->type == AST_CONS && args->type == AST_CONS)
    {
      AST *param = CAR (params);
      AST *arg_value = evaluate_expression (environment, CAR (args));

      if (param->type != AST_SYMBOL)
        return make_error ("ERROR: lambda parameter must be symbol\n");

      // Add new binding to the front of environment list
      AST *binding = make_cons (param, arg_value);
      environment->as.CONS.CDR = make_cons (binding, environment->as.CONS.CDR);

      params = CDR (params);
      args = CDR (args);
    }

  if (!IS_NULL (params) || !IS_NULL (args))
    return make_error ("ERROR: argument count mismatch in lambda call\n");

  return nil ();
}

static AST *
apply (AST *function, AST *environment, AST *arguments)
{
  if (function->type == AST_BUILTIN_SPECIAL)
    return function->as.BUILTIN (environment, arguments);

  if (function->type == AST_BUILTIN_NORMAL)
    {
      AST *evaluated_arguments = evaluate_arguments (environment, arguments);

      if (evaluated_arguments->type == AST_ERROR)
        return evaluated_arguments;

      return function->as.BUILTIN (environment, evaluated_arguments);
    }

  if (function->type == AST_LAMBDA)
    {
      AST *lambda_environment = make_cons (function->as.LAMBDA.environment, nil ());

      AST *err = bind_arguments (lambda_environment, function->as.LAMBDA.parameters, arguments);
      ERROR_OUT (err);

      AST *result = nil ();
      AST *body = function->as.LAMBDA.body;

      while (body->type == AST_CONS)
        {
          result = evaluate_expression (lambda_environment, CAR (body));
          ERROR_OUT (result);
          body = CDR (body);
        }
      return result;
    }

  return make_error ("ERROR: apply: tried to apply non-function\n");
}

static AST *
evaluate_arguments (AST *environment, AST *arguments)
{
  AST *head = nil ();
  AST *tail = NULL;

  while (arguments->type == AST_CONS)
    {
      AST *value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (value);

      AST *new_node = make_cons (value, nil ());

      if (tail)
        CDR (tail) = new_node;
      else
        head = new_node;

      tail = new_node;
      arguments = CDR (arguments);
    }

  return head;
}

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
    case AST_QUOTE:
    case AST_NIL: return expression;

    case AST_SYMBOL: return environment_get (environment, expression);

    case AST_CONS:
      {
        AST *operator_node = CAR (expression);
        AST *arguments = CDR (expression);

        AST *function = (operator_node->type == AST_SYMBOL)
                            ? environment_get (environment, operator_node)
                            : evaluate_expression (environment, operator_node);

        ERROR_OUT (function);

        if (function->type == AST_SYMBOL)
          function = environment_get (environment, function);

        return apply (function, environment, arguments);
      }

    case AST_END_OF_FILE: return expression;

    case AST_ERROR: return expression;

    default: return make_error ("ERROR: evaluate_expression: unknown AST type\n");
    }
}
