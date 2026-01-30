#include "builtins/forms.h"

#include "core/value.h"
#include "core/eval.h"
#include "core/quasiquote.h"

Val *
builtin_define (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return val_error (
        "define expects (symbol expr) or (func-name args) expr");

  Val *to_be_defined = CAR (arguments);
  Val *definition = CADR (arguments);

  // Case 1: Variable definition (define symbol expr)
  if (to_be_defined->type == VALUE_SYMBOL)
    {
      Val *current = environment_get (environment, to_be_defined);
      if (current->type != VALUE_ERROR)
        return val_error ("define: symbol already defined");

      // Initialize with nil first
      environment_set (environment, to_be_defined, nil ());

      Val *value = evaluate_expression (environment, definition);
      ERROR_OUT (value);

      // Update the binding
      environment_update (environment, to_be_defined, value);

      return to_be_defined;
    }
  // Case 2: Function definition (define (name args...) expr)
  else if (to_be_defined->type == VALUE_CONS)
    {
      if (IS_NULL (to_be_defined))
        return val_error ("define: function definition cannot be empty");

      Val *func_name = CAR (to_be_defined);
      if (func_name->type != VALUE_SYMBOL)
        return val_error ("define: function name must be a symbol");

      Val *current = environment_get (environment, func_name);
      if (current->type != VALUE_ERROR)
        return val_error ("define: symbol already defined");

      Val *params = CDR (to_be_defined);

      // Initialize with nil first
      environment_set (environment, func_name, nil ());

      Val *lambda_args = val_cons (params, CDR (arguments));
      Val *lambda = builtin_lambda (environment, lambda_args);
      ERROR_OUT (lambda);

      // Update the environment with the lambda
      environment_update (environment, func_name, lambda);

      return func_name;
    }
  else
    return val_error ("define: first argument must be a symbol or cons");
}
Val *
builtin_set (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return val_error ("set!: expects (symbol expr)");

  Val *symbol = CAR (arguments);
  if (symbol->type != VALUE_SYMBOL)
    return val_error ("set!: first argument must be a symbol");

  Val *current = environment_get (environment, symbol);
  if (current->type == VALUE_ERROR)
    return val_error ("set!: cannot set! undefined symbol");

  Val *value = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (value);

  environment_update (environment, symbol, value);

  return symbol;
}

Val *
builtin_let (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments))
    return val_error ("let: expects at levalue bindings and body");

  Val *bindings = CAR (arguments);
  Val *body = CDR (arguments);

  if (bindings->type != VALUE_CONS && bindings->type != VALUE_NIL)
    return val_error ("let: first argument must be a list of bindings");

  Val *inner_environment = val_cons (environment, nil ());

  Val *current = bindings;
  while (current->type == VALUE_CONS)
    {
      Val *binding = CAR (current);
      if (binding->type != VALUE_CONS || IS_NULL (binding)
          || IS_NULL (CDR (binding)))
        return val_error ("let: each binding must be (symbol value)");

      Val *key = CAR (binding);
      Val *value_expression = CADR (binding);

      if (key->type != VALUE_SYMBOL)
        return val_error ("let: binding first element must be symbol");

      // Initialize with nil first
      environment_set (inner_environment, key, nil ());

      // Evaluate value in parent environment
      Val *value = evaluate_expression (environment, value_expression);
      ERROR_OUT (value);

      // Update binding
      environment_update (inner_environment, key, value);

      current = CDR (current);
    }

  Val *result = nil ();
  current = body;
  while (current->type == VALUE_CONS)
    {
      result = evaluate_expression (inner_environment, CAR (current));
      ERROR_OUT (result);
      current = CDR (current);
    }

  return result;
}

Val *
builtin_let_star (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments))
    return val_error ("let*: expects at levalue bindings and body");

  Val *bindings = CAR (arguments);
  Val *body = CDR (arguments);

  if (bindings->type != VALUE_CONS && bindings->type != VALUE_NIL)
    return val_error ("let*: first argument must be a list of bindings");

  Val *inner_environment = val_cons (environment, nil ());

  Val *current = bindings;
  while (current->type == VALUE_CONS)
    {
      Val *binding = CAR (current);
      if (binding->type != VALUE_CONS || IS_NULL (binding)
          || IS_NULL (CDR (binding)))
        return val_error ("let: each binding must be (symbol value)");

      Val *key = CAR (binding);
      Val *value_expression = CADR (binding);

      if (key->type != VALUE_SYMBOL)
        return val_error ("let: binding first element must be symbol");

      environment_set (inner_environment, key, nil ());

      Val *value = evaluate_expression (inner_environment, value_expression);
      ERROR_OUT (value);

      environment_update (inner_environment, key, value);

      current = CDR (current);
    }

  Val *result = nil ();
  current = body;
  while (current->type == VALUE_CONS)
    {
      result = evaluate_expression (inner_environment, CAR (current));
      ERROR_OUT (result);
      current = CDR (current);
    }

  return result;
}

Val *
builtin_begin (Val *environment, Val *arguments)
{
  Val *lvalue_value = nil ();

  while (arguments->type == VALUE_CONS)
    {
      lvalue_value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (lvalue_value);
      arguments = CDR (arguments);
    }

  return lvalue_value;
}

Val *
builtin_eval (Val *env, Val *args)
{
  if (IS_NULL (args) || !IS_NULL (CDR (args)))
    return val_error ("eval expects exactly one argument");

  Val *expr = CAR (args);

  Val *expr_to_eval = evaluate_expression (env, expr);
  ERROR_OUT (expr_to_eval);

  return evaluate_expression (env, expr_to_eval);
}

Val *
builtin_quote (Val *environment, Val *arguments)
{
  (void)environment;

  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return val_error ("ERROR: quote expects one argument\n");

  return CAR (arguments);
}

Val *
builtin_quasiquote (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return val_error ("quasiquote expects exactly one argument");

  Val *expansion_logic = expand_quasiquote (CAR (arguments), 1);
  ERROR_OUT (expansion_logic);

  return evaluate_expression (environment, expansion_logic);
}

Val *
builtin_lambda (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments))
    return val_error ("ERROR: lambda expects at levalue parameter list\n");

  Val *parameters = CAR (arguments);
  Val *body = CDR (arguments);

  Val *lambda = malloc (sizeof (Val));
  lambda->type = VALUE_LAMBDA;
  lambda->as.LAMBDA.parameters = parameters;
  lambda->as.LAMBDA.body = body;
  lambda->as.LAMBDA.environment = environment;

  return lambda;
}
