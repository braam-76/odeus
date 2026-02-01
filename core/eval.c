#include "core/eval.h"
#include "core/value.h"

/*
 * Bind lambda arguments
 *  - call_env  : environment where arguments are evaluated
 *  - frame     : fresh lambda call frame
 */
static Val *bind_arguments (Env *call_env, Env *frame, Val *parameters,
                            Val *arguments);

/*
 * Bind macro arguments (unevaluated)
 */
static Val *bind_macro_arguments (Env *frame, Val *parameters, Val *arguments);

Val *
evaluate_expression (Env *environment, Val *expression)
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
        Val *expanded = macro_expand_expression (environment, expression);
        ERROR_OUT (expanded);

        if (expanded != expression)
          return evaluate_expression (environment, expanded);

        Val *op = CAR (expression);
        Val *args = CDR (expression);

        Val *fn = (op->type == VALUE_SYMBOL)
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
macro_expand_expression (Env *environment, Val *expr)
{
  if (expr->type != VALUE_CONS)
    return expr;

  Val *head = CAR (expr);
  if (head->type != VALUE_SYMBOL)
    return expr;

  Val *macro = env_get (environment, head);
  ERROR_OUT (macro);

  if (macro->type != VALUE_MACRO)
    return expr;

  /* fresh macro expansion frame */
  Env *frame = env_init (macro->as.MACRO.environment);

  Val *err
      = bind_macro_arguments (frame, macro->as.MACRO.parameters, CDR (expr));
  ERROR_OUT (err);

  /* evaluate macro body to produce code */
  Val *result = val_nil ();
  for (Val *body = macro->as.MACRO.body; body->type == VALUE_CONS;
       body = CDR (body))
    {
      result = evaluate_expression (frame, CAR (body));
      ERROR_OUT (result);
    }

  return result; /* returned unevaluated */
}

Val *
apply (Env *call_env, Val *function, Val *arguments)
{
  if (function->type == VALUE_BUILTIN)
    return function->as.BUILTIN (call_env, arguments);

  if (function->type == VALUE_LAMBDA)
    {
      /* fresh call frame */
      Env *frame = env_init (function->as.LAMBDA.environment);

      Val *err = bind_arguments (call_env, /* arguments evaluated here */
                                 frame,    /* bindings stored here */
                                 function->as.LAMBDA.parameters, arguments);
      ERROR_OUT (err);

      Val *result = val_nil ();
      for (Val *body = function->as.LAMBDA.body; body->type == VALUE_CONS;
           body = CDR (body))
        {
          result = evaluate_expression (frame, CAR (body));
          ERROR_OUT (result);
        }

      return result;
    }

  return val_error ("attempt to call non-function");
}

static Val *
bind_arguments (Env *call_env, Env *frame, Val *parameters, Val *arguments)
{
  Val *params = parameters;
  Val *args = arguments;

  /* fixed parameters */
  while (params->type == VALUE_CONS)
    {
      if (args->type != VALUE_CONS)
        return val_error ("lambda: too few arguments");

      Val *param = CAR (params);
      if (param->type != VALUE_SYMBOL)
        return val_error ("lambda parameter must be symbol");

      Val *value = evaluate_expression (call_env, CAR (args));
      ERROR_OUT (value);

      env_set (frame, param, value);

      params = CDR (params);
      args = CDR (args);
    }

  /* rest parameter */
  if (params->type == VALUE_SYMBOL)
    {
      Val *list = val_nil ();
      Val *tail = NULL;

      while (args->type == VALUE_CONS)
        {
          Val *value = evaluate_expression (call_env, CAR (args));
          ERROR_OUT (value);

          Val *cell = val_cons (value, val_nil ());
          if (IS_NULL (list))
            list = cell;
          else
            tail->as.CONS.CDR = cell;

          tail = cell;
          args = CDR (args);
        }

      env_set (frame, params, list);
      return val_nil ();
    }

  if (params->type != VALUE_NIL)
    return val_error ("lambda: invalid parameter list");

  if (args->type != VALUE_NIL)
    return val_error ("lambda: too many arguments");

  return val_nil ();
}

static Val *
bind_macro_arguments (Env *frame, Val *parameters, Val *arguments)
{
  Val *params = parameters;
  Val *args = arguments;

  while (params->type == VALUE_CONS)
    {
      if (args->type != VALUE_CONS)
        return val_error ("macro: too few arguments");

      Val *param = CAR (params);
      if (param->type != VALUE_SYMBOL)
        return val_error ("macro parameter must be symbol");

      env_set (frame, param, CAR (args));
      params = CDR (params);
      args = CDR (args);
    }

  if (params->type == VALUE_SYMBOL)
    {
      env_set (frame, params, args);
      return val_nil ();
    }

  if (params->type != VALUE_NIL)
    return val_error ("macro: invalid parameter list");

  if (args->type != VALUE_NIL)
    return val_error ("macro: too many arguments");

  return val_nil ();
}
