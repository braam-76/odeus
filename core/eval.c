#include "core/eval.h"

static AST *bind_arguments (AST *environment, AST *params, AST *args);

static AST *expand_quasiquote_list (AST *list, int depth);

static int is_symbol_named (AST *node, const char *name);
static int is_unquote (AST *node);
static int is_unquote_splicing (AST *node);

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

        // evaluate operator (may be a symbol or lambda)
        AST *function = (operator_node->type == AST_SYMBOL)
                            ? environment_get (environment, operator_node)
                            : evaluate_expression (environment, operator_node);

        ERROR_OUT (function);

        if (function->type == AST_SYMBOL)
          function = environment_get (environment, function);

        return apply (function, environment, arguments);
      }

    case AST_END_OF_FILE:
    case AST_ERROR: return expression;

    default: return make_error ("ERROR: evaluate_expression: unknown AST type\n");
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
expand_quasiquote (AST *expr, int depth)
{
  if (!expr)
    return nil ();

  if (expr->type != AST_CONS)
    return expr;

  if (is_unquote (expr))
    {
      if (depth == 1)
        return CADR (expr);
      else
        return make_cons (make_symbol ("unquote"),
                          make_cons (expand_quasiquote (CADR (expr), depth - 1), nil ()));
    }

  if (is_unquote_splicing (expr) && depth == 1)
    return make_error ("unquote-splicing not in list context");

  if (is_symbol_named (CAR (expr), "quasiquote"))
    return make_cons (make_symbol ("quasiquote"),
                      make_cons (expand_quasiquote (CADR (expr), depth + 1), nil ()));

  return expand_quasiquote_list (expr, depth);
}

static AST *
expand_quasiquote_list (AST *list, int depth)
{
  if (IS_NULL (list))
    return make_cons (make_symbol ("quote"), make_cons (nil (), nil ()));

  AST *before_splice = nil ();
  AST *last_before = NULL;
  AST *current = list;

  // Collect elements until we hit an unquote-splicing or end of list
  while (current->type == AST_CONS)
    {
      AST *head = CAR (current);

      // Check if this is an unquote-splicing at the right depth
      if (is_unquote_splicing (head) && depth == 1)
        {
          AST *splice_expr = CADR (head);
          AST *rest = expand_quasiquote_list (CDR (current), depth);

          if (IS_NULL (before_splice))
            return make_cons (make_symbol ("append"),
                              make_cons (splice_expr, make_cons (rest, nil ())));

          // We have elements before the splice
          // Build (list elem1 elem2 ...) for elements before splice
          AST *list_expr = make_cons (make_symbol ("list"), before_splice);

          // If rest is '(), we don't need to append it
          if (is_symbol_named (rest, "quote") && IS_NULL (CADR (rest)))
            return make_cons (make_symbol ("append"),
                              make_cons (list_expr, make_cons (splice_expr, nil ())));

          return make_cons (
              make_symbol ("append"),
              make_cons (list_expr,
                         make_cons (make_cons (make_symbol ("append"),
                                               make_cons (splice_expr, make_cons (rest, nil ()))),
                                    nil ())));
        }

      // Not a splice, so expand this element and add to before_splice list
      AST *expanded = expand_quasiquote (head, depth);
      AST *new_cons = make_cons (expanded, nil ());

      if (IS_NULL (before_splice))
        before_splice = new_cons;
      else
        last_before->as.CONS.CDR = new_cons;
      last_before = new_cons;

      current = CDR (current);
    }

  // If we get here, there were no unquote-splicings in the list
  // Build (list elem1 elem2 ...) for all elements
  return make_cons (make_symbol ("list"), before_splice);
}

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

AST *
apply (AST *function, AST *environment, AST *arguments)
{
  if (function->type == AST_BUILTIN_SPECIAL)
    {
      AST *expanded = function->as.BUILTIN (environment, arguments);
      ERROR_OUT (expanded);
      return expanded;
    }

  if (function->type == AST_BUILTIN_NORMAL)
    return function->as.BUILTIN (environment, arguments);

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

int
is_symbol_named (AST *node, const char *name)
{
  return node->type == AST_SYMBOL && strcmp (node->as.SYMBOL, name) == 0;
}

int
is_unquote (AST *node)
{
  return node->type == AST_CONS && is_symbol_named (CAR (node), "unquote");
}

int
is_unquote_splicing (AST *node)
{
  return node->type == AST_CONS && is_symbol_named (CAR (node), "unquote-splicing")
         && !IS_NULL (CDR (node)) && IS_NULL (CDDR (node));
}
