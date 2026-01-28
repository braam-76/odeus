#include "builtins/forms.h"

#include "core/ast.h"
#include "core/eval.h"
#include "core/quasiquote.h"

AST *
builtin_define (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return make_error (
        "define expects (symbol expr) or (func-name args) expr");

  AST *to_be_defined = CAR (arguments);
  AST *definition = CADR (arguments);

  // Case 1: Variable definition (define symbol expr)
  if (to_be_defined->type == AST_SYMBOL)
    {
      AST *current = environment_get (environment, to_be_defined);
      if (current->type != AST_ERROR)
        return make_error ("define: symbol already defined");

      // Initialize with nil first
      environment_set (environment, to_be_defined, nil ());

      AST *value = evaluate_expression (environment, definition);
      ERROR_OUT (value);

      // Update the binding
      environment_update (environment, to_be_defined, value);

      return to_be_defined;
    }
  // Case 2: Function definition (define (name args...) expr)
  else if (to_be_defined->type == AST_CONS)
    {
      if (IS_NULL (to_be_defined))
        return make_error ("define: function definition cannot be empty");

      AST *func_name = CAR (to_be_defined);
      if (func_name->type != AST_SYMBOL)
        return make_error ("define: function name must be a symbol");

      AST *current = environment_get (environment, func_name);
      if (current->type != AST_ERROR)
        return make_error ("define: symbol already defined");

      AST *params = CDR (to_be_defined);

      // Initialize with nil first
      environment_set (environment, func_name, nil ());

      AST *lambda_args = make_cons (params, CDR (arguments));
      AST *lambda = builtin_lambda (environment, lambda_args);
      ERROR_OUT (lambda);

      // Update the environment with the lambda
      environment_update (environment, func_name, lambda);

      return func_name;
    }
  else
    return make_error ("define: first argument must be a symbol or cons");
}
AST *
builtin_set (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return make_error ("set!: expects (symbol expr)");

  AST *symbol = CAR (arguments);
  if (symbol->type != AST_SYMBOL)
    return make_error ("set!: first argument must be a symbol");

  AST *current = environment_get (environment, symbol);
  if (current->type == AST_ERROR)
    return make_error ("set!: cannot set! undefined symbol");

  AST *value = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (value);

  environment_update (environment, symbol, value);

  return symbol;
}

AST *
builtin_let (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("let: expects at least bindings and body");

  AST *bindings = CAR (arguments);
  AST *body = CDR (arguments);

  if (bindings->type != AST_CONS && bindings->type != AST_NIL)
    return make_error ("let: first argument must be a list of bindings");

  AST *inner_environment = make_cons (environment, nil ());

  AST *current = bindings;
  while (current->type == AST_CONS)
    {
      AST *binding = CAR (current);
      if (binding->type != AST_CONS || IS_NULL (binding)
          || IS_NULL (CDR (binding)))
        return make_error ("let: each binding must be (symbol value)");

      AST *key = CAR (binding);
      AST *value_expression = CADR (binding);

      if (key->type != AST_SYMBOL)
        return make_error ("let: binding first element must be symbol");

      // Initialize with nil first
      environment_set (inner_environment, key, nil ());

      // Evaluate value in parent environment
      AST *value = evaluate_expression (environment, value_expression);
      ERROR_OUT (value);

      // Update binding
      environment_update (inner_environment, key, value);

      current = CDR (current);
    }

  AST *result = nil ();
  current = body;
  while (current->type == AST_CONS)
    {
      result = evaluate_expression (inner_environment, CAR (current));
      ERROR_OUT (result);
      current = CDR (current);
    }

  return result;
}

AST *
builtin_let_star (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("let*: expects at least bindings and body");

  AST *bindings = CAR (arguments);
  AST *body = CDR (arguments);

  if (bindings->type != AST_CONS && bindings->type != AST_NIL)
    return make_error ("let*: first argument must be a list of bindings");

  AST *inner_environment = make_cons (environment, nil ());

  AST *current = bindings;
  while (current->type == AST_CONS)
    {
      AST *binding = CAR (current);
      if (binding->type != AST_CONS || IS_NULL (binding)
          || IS_NULL (CDR (binding)))
        return make_error ("let: each binding must be (symbol value)");

      AST *key = CAR (binding);
      AST *value_expression = CADR (binding);

      if (key->type != AST_SYMBOL)
        return make_error ("let: binding first element must be symbol");

      environment_set (inner_environment, key, nil ());

      AST *value = evaluate_expression (inner_environment, value_expression);
      ERROR_OUT (value);

      environment_update (inner_environment, key, value);

      current = CDR (current);
    }

  AST *result = nil ();
  current = body;
  while (current->type == AST_CONS)
    {
      result = evaluate_expression (inner_environment, CAR (current));
      ERROR_OUT (result);
      current = CDR (current);
    }

  return result;
}

AST *
builtin_begin (AST *environment, AST *arguments)
{
  AST *last_value = nil ();

  while (arguments->type == AST_CONS)
    {
      last_value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (last_value);
      arguments = CDR (arguments);
    }

  return last_value;
}

AST *
builtin_eval (AST *env, AST *args)
{
  if (IS_NULL (args) || !IS_NULL (CDR (args)))
    return make_error ("eval expects exactly one argument");

  AST *expr = CAR (args);

  AST *expr_to_eval = evaluate_expression (env, expr);
  ERROR_OUT (expr_to_eval);

  return evaluate_expression (env, expr_to_eval);
}

AST *
builtin_quote (AST *environment, AST *arguments)
{
  (void)environment;

  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("ERROR: quote expects one argument\n");

  return CAR (arguments);
}

AST *
builtin_quasiquote (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("quasiquote expects exactly one argument");

  AST *expansion_logic = expand_quasiquote (CAR (arguments), 1);
  ERROR_OUT (expansion_logic);

  return evaluate_expression (environment, expansion_logic);
}

AST *
builtin_lambda (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("ERROR: lambda expects at least parameter list\n");

  AST *parameters = CAR (arguments);
  AST *body = CDR (arguments);

  AST *lambda = malloc (sizeof (AST));
  lambda->type = AST_LAMBDA;
  lambda->as.LAMBDA.parameters = parameters;
  lambda->as.LAMBDA.body = body;
  lambda->as.LAMBDA.environment = environment;

  return lambda;
}
