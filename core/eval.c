#include "core/eval.h"
#include "core/value.h"

static Value *bind_arguments (Environment *call_env, Environment *frame, Value *parameters,
                            Value *arguments);

static Value *bind_macro_arguments (Environment *frame, Value *parameters, Value *arguments);

Value *
evaluate_expression (Environment *environment, Value *expression)
{
  if (!expression)
    return val_nil ();

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
      return env_get (environment, expression);

    case VALUE_CONS:
      {
        Value *expanded = macro_expand_expression (environment, expression);
        ERROR_OUT (expanded);

        if (expanded != expression)
          return evaluate_expression (environment, expanded);

        Value *op = CAR (expression);
        Value *args = CDR (expression);

        Value *fn = (op->type == VALUE_SYMBOL)
                      ? env_get (environment, op)
                      : evaluate_expression (environment, op);

        ERROR_OUT (fn);

        return apply (environment, fn, args);
      }

    case VALUE_ERROR:
    case VALUE_END_OF_FILE:
      return expression;

    default:
      return val_error ("evaluate_expression: unknown VALUE type");
    }
}

int
arguments_length (Value *arguments)
{
  int count = 0;

  while (arguments->type == VALUE_CONS)
    {
      count++;
      arguments = CDR (arguments);
    }

  return count;
}

Value *
macro_expand_expression (Environment *environment, Value *expr)
{
  if (expr->type != VALUE_CONS)
    return expr;

  Value *head = CAR (expr);
  if (head->type != VALUE_SYMBOL)
    return expr;

  Value *macro = env_get (environment, head);
  ERROR_OUT (macro);

  if (macro->type != VALUE_MACRO)
    return expr;

  Environment *frame = env_init (macro->as.CLOSURE.environment);

  Value *err
      = bind_macro_arguments (frame, macro->as.CLOSURE.parameters, CDR (expr));
  ERROR_OUT (err);

  Value *result = val_nil ();
  for (Value *body = macro->as.CLOSURE.body; body->type == VALUE_CONS;
       body = CDR (body))
    {
      result = evaluate_expression (frame, CAR (body));
      ERROR_OUT (result);
    }

  return result;
}

Value *
apply (Environment *call_env, Value *function, Value *arguments)
{
  if (function->type == VALUE_BUILTIN)
    return function->as.BUILTIN (call_env, arguments);

  if (function->type == VALUE_LAMBDA)
    {
      Environment *frame = env_init (function->as.CLOSURE.environment);

      Value *err = bind_arguments (call_env, frame,
                                 function->as.CLOSURE.parameters, arguments);
      ERROR_OUT (err);

      Value *result = val_nil ();
      for (Value *body = function->as.CLOSURE.body; body->type == VALUE_CONS;
           body = CDR (body))
        {
          result = evaluate_expression (frame, CAR (body));
          ERROR_OUT (result);
        }

      return result;
    }

  return val_error ("attempt to call non-function");
}

static Value *
bind_arguments (Environment *call_env, Environment *frame, Value *parameters, Value *arguments)
{
  Value *params = parameters;
  Value *args = arguments;

  /* fixed parameters */
  while (params->type == VALUE_CONS)
    {
      if (args->type != VALUE_CONS)
        return val_error ("lambda: too few arguments");

      Value *param = CAR (params);
      if (param->type != VALUE_SYMBOL)
        return val_error ("lambda parameter must be symbol");

      Value *value = evaluate_expression (call_env, CAR (args));
      ERROR_OUT (value);

      env_set (frame, param, value, param->meta);

      params = CDR (params);
      args = CDR (args);
    }

  /* rest parameter */
  if (params->type == VALUE_SYMBOL)
    {
      Value *list = val_nil ();
      Value *tail = NULL;

      while (args->type == VALUE_CONS)
        {
          Value *value = evaluate_expression (call_env, CAR (args));
          ERROR_OUT (value);

          Value *cell = val_cons (value, val_nil ());
          if (IS_NULL (list))
            list = cell;
          else
            tail->as.CONS.CDR = cell;

          tail = cell;
          args = CDR (args);
        }

      env_set (frame, params, list, params->meta);
      return val_nil ();
    }

  if (params->type != VALUE_NIL)
    return val_error ("lambda: invalid parameter list");

  if (args->type != VALUE_NIL)
    return val_error ("lambda: too many arguments");

  return val_nil ();
}

static Value *
bind_macro_arguments (Environment *frame, Value *parameters, Value *arguments)
{
  Value *params = parameters;
  Value *args = arguments;

  while (params->type == VALUE_CONS)
    {
      if (args->type != VALUE_CONS)
        return val_error ("macro: too few arguments");

      Value *param = CAR (params);
      if (param->type != VALUE_SYMBOL)
        return val_error ("macro parameter must be symbol");

      env_set (frame, param, CAR (args), param->meta);
      params = CDR (params);
      args = CDR (args);
    }

  if (params->type == VALUE_SYMBOL)
    {
      env_set (frame, params, args, params->meta);
      return val_nil ();
    }

  if (params->type != VALUE_NIL)
    return val_error ("macro: invalid parameter list");

  if (args->type != VALUE_NIL)
    return val_error ("macro: too many arguments");

  return val_nil ();
}
