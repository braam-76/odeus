#include "builtins/list.h"

#include "core/value.h"
#include "core/eval.h"

Val *
builtin_apply (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) < 2)
    return make_error (
        "apply: expected at levalue 2 arguments");

  Val *fn = CAR (arguments);
  Val *arg_list = CADR (arguments);

  if (arg_list->type != VALUE_NIL && arg_list->type != VALUE_CONS)
    return make_error ("apply: second argument must be a list");

  return apply (fn, environment, arg_list);
}

Val *
builtin_cons (Val *environment, Val *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CADR (arguments))
      || !IS_NULL (CDDR (arguments)))
    return make_error ("ERROR: cons expects two argument\n");

  Val *first_argument = evaluate_expression (environment, CAR (arguments));
  Val *second_argument = evaluate_expression (environment, CADR (arguments));

  return make_cons (first_argument, second_argument);
}

Val *
builtin_list (Val *environment, Val *arguments)
{
  Val *result = nil ();
  Val *lvalue = NULL;

  while (arguments->type == VALUE_CONS)
    {
      Val *elem = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (elem);

      Val *new_cons = make_cons (elem, nil ());
      if (IS_NULL (result))
        result = new_cons;
      else
        lvalue->as.CONS.CDR = new_cons;

      lvalue = new_cons;
      arguments = CDR (arguments);
    }

  if (!IS_NULL (arguments))
    return make_error ("list: improper list in arguments");

  return result;
}

Val *
builtin_car (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("car: expects exactly one argument");

  Val *list = evaluate_expression (environment, CAR (arguments));

  if (list->type != VALUE_CONS)
    return make_error ("car: argument is not a pair");

  return CAR (list);
}

Val *
builtin_cdr (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("cdr: expects exactly one argument");

  Val *list = evaluate_expression (environment, CAR (arguments));

  if (list->type != VALUE_CONS)
    return make_error ("cdr: argument is not a pair");

  return CDR (list);
}

Val *
builtin_set_car (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("set-car!: expects exactly two argument");

  Val *list = evaluate_expression (environment, CAR (arguments));
  Val *value = evaluate_expression (environment, CADR (arguments));

  if (list->type != VALUE_CONS)
    return make_error ("set-car!: first argument is not a pair");

  CAR (list) = value;

  return list;
}

Val *
builtin_set_cdr (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("set-cdr!: expects exactly two argument");

  Val *list = evaluate_expression (environment, CAR (arguments));
  Val *value = evaluate_expression (environment, CADR (arguments));

  if (list->type != VALUE_CONS)
    return make_error ("set-cdr!: first argument is not a pair");

  CDR (list) = value;

  return list;
}

Val *
builtin_length (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("length: expects exactly one argument");

  Val *list = evaluate_expression (environment, CAR (arguments));

  int count = 0;
  while (list->type == VALUE_CONS)
    {
      count++;
      list = CDR (list);
    };

  return make_integer (count);
}

Val *
builtin_reverse (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("reverse: expects exactly one argument");

  Val *list = evaluate_expression (environment, CAR (arguments));
  Val *result = nil ();

  while (list->type == VALUE_CONS)
    {
      result = make_cons (CAR (list), result);
      list = CDR (list);
    }

  return result;
}

Val *
builtin_filter (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("filter: expects exactly two arguments");

  Val *function = evaluate_expression (environment, CAR (arguments));

  if (function->type != VALUE_BUILTIN && function->type != VALUE_LAMBDA)
    return make_error ("filter: first argument must be a function");

  Val *list = evaluate_expression (environment, CADR (arguments));
  if (list->type != VALUE_CONS)
    return make_error ("filter: second argument should be list/cons");

  Val *result_head = make_cons (nil (), nil ());
  Val *result_tail = result_head;

  while (list->type == VALUE_CONS && !IS_NULL (list))
    {
      Val *current_element = CAR (list);
      Val *function_arguments = make_cons (current_element, nil ());
      Val *predicate_result
          = apply (function, environment, function_arguments);
      ERROR_OUT (predicate_result);

      if (!IS_NULL (predicate_result))
        {
          Val *new_cell = make_cons (current_element, nil ());

          CDR (result_tail) = new_cell;
          result_tail = new_cell;
        }

      list = CDR (list);
    }

  return CDR (result_head);
}
