#include "builtins/list.h"

#include "core/ast.h"
#include "core/eval.h"

AST *
builtin_cons (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CADR (arguments))
      || !IS_NULL (CDDR (arguments)))
    return make_error ("ERROR: cons expects two argument\n");

  AST *first_argument = evaluate_expression (environment, CAR (arguments));
  AST *second_argument = evaluate_expression (environment, CADR (arguments));

  return make_cons (first_argument, second_argument);
}

AST *
builtin_list (AST *environment, AST *arguments)
{
  AST *result = nil ();
  AST *last = NULL;

  while (arguments->type == AST_CONS)
    {
      AST *elem = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (elem);

      AST *new_cons = make_cons (elem, nil ());
      if (IS_NULL (result))
        result = new_cons;
      else
        last->as.CONS.CDR = new_cons;

      last = new_cons;
      arguments = CDR (arguments);
    }

  if (!IS_NULL (arguments))
    return make_error ("list: improper list in arguments");

  return result;
}

AST *
builtin_car (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("car: expects exactly one argument");

  AST *list = evaluate_expression (environment, CAR (arguments));

  if (list->type != AST_CONS)
    return make_error ("car: argument is not a pair");

  return CAR (list);
}

AST *
builtin_cdr (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("cdr: expects exactly one argument");

  AST *list = evaluate_expression (environment, CAR (arguments));

  if (list->type != AST_CONS)
    return make_error ("cdr: argument is not a pair");

  return CDR (list);
}

AST *
builtin_set_car (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("set-car!: expects exactly two argument");

  AST *list = evaluate_expression (environment, CAR (arguments));
  AST *value = evaluate_expression (environment, CADR (arguments));

  if (list->type != AST_CONS)
    return make_error ("set-car!: first argument is not a pair");

  CAR (list) = value;

  return list;
}

AST *
builtin_set_cdr (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("set-cdr!: expects exactly two argument");

  AST *list = evaluate_expression (environment, CAR (arguments));
  AST *value = evaluate_expression (environment, CADR (arguments));

  if (list->type != AST_CONS)
    return make_error ("set-cdr!: first argument is not a pair");

  CDR (list) = value;

  return list;
}

AST *
builtin_length (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("length: expects exactly one argument");

  AST *list = evaluate_expression (environment, CAR (arguments));

  int count = 0;
  while (list->type == AST_CONS)
    {
      count++;
      list = CDR (list);
    };

  return make_integer (count);
}

AST *
builtin_reverse (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("reverse: expects exactly one argument");

  AST *list = evaluate_expression (environment, CAR (arguments));
  AST *result = nil ();

  while (list->type == AST_CONS)
    {
      result = make_cons (CAR (list), result);
      list = CDR (list);
    }

  return result;
}

AST *
builtin_filter (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("filter: expects exactly two arguments");

  AST *function = evaluate_expression (environment, CAR (arguments));

  if (function->type != AST_BUILTIN_NORMAL
      && function->type != AST_BUILTIN_SPECIAL && function->type != AST_LAMBDA)
    return make_error ("filter: first argument must be a function");

  AST *list = evaluate_expression (environment, CADR (arguments));
  if (list->type != AST_CONS)
    return make_error ("filter: second argument should be list/cons");

  AST *result_head = make_cons (nil (), nil ());
  AST *result_tail = result_head;

  while (list->type == AST_CONS && !IS_NULL (list))
    {
      AST *current_element = CAR (list);
      AST *function_arguments = make_cons (current_element, nil ());
      AST *predicate_result
          = apply (function, environment, function_arguments);
      ERROR_OUT (predicate_result);

      if (!IS_NULL (predicate_result))
        {
          AST *new_cell = make_cons (current_element, nil ());

          CDR (result_tail) = new_cell;
          result_tail = new_cell;
        }

      list = CDR (list);
    }

  return CDR (result_head);
}
