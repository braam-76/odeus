#include "core/eval.h"
#include "core/value.h"

static Val *bind_arguments (Val *frame, Val *caller_environment,
                            Val *parameters, Val *arguments);
static Val *bind_macro_arguments (Val *frame, Val *parameters, Val *arguments);

Val *
evaluate_expression (Val *environment, Val *expression)
{
  if (!expression)
    return nil ();

  switch (expression->type)
    {
    case VALUE_INTEGER:
    case VALUE_FLOAT:
    case VALUE_STRING:
    case VALUE_NIL:
    case VALUE_LAMBDA:
    case VALUE_MACRO:
      return expression;

    case VALUE_SYMBOL:
      return environment_get (environment, expression);

    case VALUE_CONS:
      {
        Val *expanded = macro_expand_expression (environment, expression);
        ERROR_OUT (expanded);

        if (expanded != expression)
          return evaluate_expression (environment, expanded);

        Val *operator_node = CAR (expression);
        Val *arguments = CDR (expression);

        Val *function = (operator_node->type == VALUE_SYMBOL)
                            ? environment_get (environment, operator_node)
                            : evaluate_expression (environment, operator_node);

        ERROR_OUT (function);

        return apply (function, environment, arguments);
      }

    case VALUE_END_OF_FILE:
    case VALUE_ERROR:
      return expression;

    default:
      return make_error ("evaluate_expression: unknown VALUE type");
    }
}

int
arguments_length (Val *arguments)
{
  int count = 0;

  while (arguments->type == VALUE_CONS)
    {
      count++;
      arguments = CDR (arguments);
    }

  return count;
}

Val *
macro_expand_expression (Val *environment, Val *expr)
{
  if (expr->type != VALUE_CONS)
    return expr;

  Val *head = CAR (expr);
  if (head->type != VALUE_SYMBOL)
    return expr;

  Val *macro = environment_get (environment, head);
  ERROR_OUT (macro);

  if (macro->type != VALUE_MACRO)
    return expr;

  // Create macro frame
  Val *frame = make_cons (macro->as.MACRO.environment, nil ());

  Val *err
      = bind_macro_arguments (frame, macro->as.MACRO.parameters, CDR (expr));
  ERROR_OUT (err);

  // Evaluate macro body *to produce code*
  Val *expanded = nil ();
  Val *body = macro->as.MACRO.body;

  while (body->type == VALUE_CONS)
    {
      expanded = evaluate_expression (frame, CAR (body));
      ERROR_OUT (expanded);
      body = CDR (body);
    }

  return expanded; // <-- NOT evaluated here
}

static Val *
bind_arguments (Val *frame, Val *caller_environment, Val *parameters,
                Val *arguments)
{
  Val *params = parameters;
  Val *args = arguments;

  // Bind fixed parameters
  while (params->type == VALUE_CONS)
    {
      if (args->type != VALUE_CONS)
        return make_error ("lambda: too few arguments");

      Val *param = CAR (params);
      if (param->type != VALUE_SYMBOL)
        return make_error ("lambda parameter must be symbol");

      Val *value = evaluate_expression (caller_environment, CAR (args));
      ERROR_OUT (value);

      environment_set (frame, param, value);

      params = CDR (params);
      args = CDR (args);
    }

  // params is now either NIL or a SYMBOL (rest parameter)

  if (params->type == VALUE_SYMBOL)
    {
      Val *evaluated_args = nil ();
      Val *lvalue = NULL;

      while (args->type == VALUE_CONS)
        {
          Val *value = evaluate_expression (caller_environment, CAR (args));
          ERROR_OUT (value);

          Val *new_cons = make_cons (value, nil ());
          if (IS_NULL (evaluated_args))
            evaluated_args = new_cons;
          else
            lvalue->as.CONS.CDR = new_cons;
          lvalue = new_cons;

          args = CDR (args);
        }

      environment_set (frame, params, evaluated_args);
      return nil ();
    }

  if (params->type != VALUE_NIL)
    return make_error ("lambda: invalid parameter list");

  if (args->type != VALUE_NIL)
    return make_error ("lambda: too many arguments");

  return nil ();
}

Val *
apply (Val *function, Val *caller_env, Val *arguments)
{
  if (function->type == VALUE_BUILTIN)
    return function->as.BUILTIN (caller_env, arguments);

  if (function->type == VALUE_LAMBDA)
    {
      Val *frame = make_cons (function->as.LAMBDA.environment, nil ());
      Val *err = bind_arguments (frame, caller_env,
                                 function->as.LAMBDA.parameters, arguments);
      ERROR_OUT (err);

      Val *result = nil ();
      Val *body = function->as.LAMBDA.body;
      while (body->type == VALUE_CONS)
        {
          result = evaluate_expression (frame, CAR (body));
          ERROR_OUT (result);
          body = CDR (body);
        }
      return result;
    }

  return make_error ("attempt to call non-function");
}

static Val *
bind_macro_arguments (Val *frame, Val *parameters, Val *arguments)
{
  Val *params = parameters;
  Val *args = arguments;

  while (params->type == VALUE_CONS)
    {
      if (args->type != VALUE_CONS)
        return make_error ("macro: too few arguments");

      Val *param = CAR (params);
      if (param->type != VALUE_SYMBOL)
        return make_error ("macro parameter must be symbol");

      environment_set (frame, param, CAR (args)); // raw VALUE
      params = CDR (params);
      args = CDR (args);
    }

  // If the lvalue parameter is a symbol, treat it as "rest"
  if (params->type == VALUE_SYMBOL)
    {
      environment_set (frame, params, args); // bind all remaining args
      return nil ();
    }

  if (params->type != VALUE_NIL)
    return make_error ("macro: invalid parameter list");

  if (args->type != VALUE_NIL)
    return make_error ("macro: too many arguments");

  return nil ();
}
