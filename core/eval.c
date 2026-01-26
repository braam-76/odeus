#include "core/eval.h"
#include "core/ast.h"

static AST *bind_arguments (AST *frame, AST *caller_environment,
                            AST *parameters, AST *arguments);
static AST *bind_macro_arguments (AST *frame, AST *parameters, AST *arguments);

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
    case AST_NIL:
    case AST_LAMBDA:
    case AST_MACRO:
      return expression;

    case AST_SYMBOL:
      return environment_get (environment, expression);

    case AST_CONS:
      {
        AST *operator_node = CAR (expression);
        AST *arguments = CDR (expression);

        AST *function = (operator_node->type == AST_SYMBOL)
                            ? environment_get (environment, operator_node)
                            : evaluate_expression (environment, operator_node);

        ERROR_OUT (function);

        return apply (function, environment, arguments);
      }

    case AST_END_OF_FILE:
    case AST_ERROR:
      return expression;

    default:
      return make_error ("evaluate_expression: unknown AST type");
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
bind_arguments (AST *frame, AST *caller_environment, AST *parameters,
                AST *arguments)
{
  AST *params = parameters;
  AST *args = arguments;

  // Bind fixed parameters
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

  // params is now either NIL or a SYMBOL (rest parameter)

  if (params->type == AST_SYMBOL)
    {
      AST *evaluated_args = nil ();
      AST *last = NULL;

      while (args->type == AST_CONS)
        {
          AST *value = evaluate_expression (caller_environment, CAR (args));
          ERROR_OUT (value);

          AST *new_cons = make_cons (value, nil ());
          if (IS_NULL (evaluated_args))
            evaluated_args = new_cons;
          else
            last->as.CONS.CDR = new_cons;
          last = new_cons;

          args = CDR (args);
        }

      environment_set (frame, params, evaluated_args);
      return nil ();
    }

  if (params->type != AST_NIL)
    return make_error ("lambda: invalid parameter list");

  if (args->type != AST_NIL)
    return make_error ("lambda: too many arguments");

  return nil ();
}

static AST *substitute_macro_parameters (AST *expr, AST *frame);

AST *
apply (AST *function, AST *caller_environment, AST *arguments)
{
  if (function->type == AST_MACRO)
    {
      AST *frame = make_cons (function->as.MACRO.environment, nil ());

      AST *err = bind_macro_arguments (frame, function->as.MACRO.parameters,
                                       arguments);
      ERROR_OUT (err);

      // Evaluate macro body to get expansion
      AST *expanded = nil ();
      AST *current = function->as.MACRO.body;

      while (current->type == AST_CONS)
        {
          expanded = evaluate_expression (frame, CAR (current));
          ERROR_OUT (expanded);
          current = CDR (current);
        }

      // Now evaluate the expansion
      return evaluate_expression (caller_environment, expanded);
    }

  // Builtin special forms
  if (function->type == AST_BUILTIN_SPECIAL)
    {
      AST *result = function->as.BUILTIN (caller_environment, arguments);
      ERROR_OUT (result);
      return result;
    }

  // Normal builtins
  if (function->type == AST_BUILTIN_NORMAL)
    {
      AST *result = function->as.BUILTIN (caller_environment, arguments);
      ERROR_OUT (result);
      return result;
    }

  // Lambda
  if (function->type == AST_LAMBDA)
    {
      AST *frame = make_cons (function->as.LAMBDA.environment, nil ());

      AST *err = bind_arguments (frame, caller_environment,
                                 function->as.LAMBDA.parameters, arguments);
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

static AST *
bind_macro_arguments (AST *frame, AST *parameters, AST *arguments)
{
  AST *params = parameters;
  AST *args = arguments;

  while (params->type == AST_CONS)
    {
      if (args->type != AST_CONS)
        return make_error ("macro: too few arguments");

      AST *param = CAR (params);
      if (param->type != AST_SYMBOL)
        return make_error ("macro parameter must be symbol");

      environment_set (frame, param, CAR (args)); // raw AST
      params = CDR (params);
      args = CDR (args);
    }

  // If the last parameter is a symbol, treat it as "rest"
  if (params->type == AST_SYMBOL)
    {
      environment_set (frame, params, args); // bind all remaining args
      return nil ();
    }

  if (params->type != AST_NIL)
    return make_error ("macro: invalid parameter list");

  if (args->type != AST_NIL)
    return make_error ("macro: too many arguments");

  return nil ();
}

// Recursively walk AST and replace symbols that match macro parameters
static AST *
substitute_macro_parameters (AST *expr, AST *frame)
{
  if (!expr)
    return nil ();

  switch (expr->type)
    {
    case AST_SYMBOL:
      {
        AST *val = environment_get (frame, expr);
        if (val->type != AST_ERROR)
          return val; // replace with AST argument
        return expr;  // not a macro parameter, keep as-is
      }

    case AST_CONS:
      {
        AST *new_car = substitute_macro_parameters (CAR (expr), frame);
        AST *new_cdr = substitute_macro_parameters (CDR (expr), frame);
        return make_cons (new_car, new_cdr);
      }

    default:
      return expr; // numbers, strings, nil, etc.
    }
}
