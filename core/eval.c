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
        AST *expanded = macro_expand_expression (environment, expression);
        ERROR_OUT (expanded);

        if (expanded != expression)
          return evaluate_expression (environment, expanded);

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

AST *
macro_expand_expression (AST *environment, AST *expr)
{
  if (expr->type != AST_CONS)
    return expr;

  AST *head = CAR (expr);
  if (head->type != AST_SYMBOL)
    return expr;

  AST *macro = environment_get (environment, head);
  ERROR_OUT (macro);

  if (macro->type != AST_MACRO)
    return expr;

  // Create macro frame
  AST *frame = make_cons (macro->as.MACRO.environment, nil ());

  AST *err
      = bind_macro_arguments (frame, macro->as.MACRO.parameters, CDR (expr));
  ERROR_OUT (err);

  // Evaluate macro body *to produce code*
  AST *expanded = nil ();
  AST *body = macro->as.MACRO.body;

  while (body->type == AST_CONS)
    {
      expanded = evaluate_expression (frame, CAR (body));
      ERROR_OUT (expanded);
      body = CDR (body);
    }

  return expanded; // <-- NOT evaluated here
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

AST *
apply (AST *function, AST *caller_env, AST *arguments)
{
  if (function->type == AST_BUILTIN)
    return function->as.BUILTIN (caller_env, arguments);

  if (function->type == AST_LAMBDA)
    {
      AST *frame = make_cons (function->as.LAMBDA.environment, nil ());
      AST *err = bind_arguments (frame, caller_env,
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
