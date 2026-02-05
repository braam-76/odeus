#include "builtins/list.h"

#include "core/eval.h"
#include "core/value.h"

Value *
builtin_apply (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) < 2)
    return val_error ("apply: expected at levalue 2 arguments");

  Value *fn = CAR (arguments);
  Value *arg_list = CADR (arguments);

  if (arg_list->type != VALUE_NIL && arg_list->type != VALUE_CONS)
    return val_error ("apply: second argument must be a list");

  return apply (environment, fn, arg_list);
}

Value *
builtin_cons (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 2)
    return val_error ("ERROR: cons expects two argument\n");

  Value *first_argument = evaluate_expression (environment, CAR (arguments));
  Value *second_argument = evaluate_expression (environment, CADR (arguments));

  return val_cons (first_argument, second_argument);
}

Value *
builtin_list (Environment *environment, Value *arguments)
{
  Value *result = val_nil ();
  Value *lvalue = NULL;

  while (arguments->type == VALUE_CONS)
    {
      Value *elem = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (elem);

      Value *new_cons = val_cons (elem, val_nil ());
      if (IS_NULL (result))
        result = new_cons;
      else
        lvalue->as.CONS.CDR = new_cons;

      lvalue = new_cons;
      arguments = CDR (arguments);
    }

  if (!IS_NULL (arguments))
    return val_error ("list: improper list in arguments");

  return result;
}

Value *
builtin_car (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("car: expects exactly one argument");

  Value *list = evaluate_expression (environment, CAR (arguments));

  if (list->type != VALUE_CONS)
    return val_error ("car: argument is not a pair");

  return CAR (list);
}

Value *
builtin_cdr (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("cdr: expects exactly one argument");

  Value *list = evaluate_expression (environment, CAR (arguments));

  if (list->type != VALUE_CONS)
    return val_error ("cdr: argument is not a pair");

  return CDR (list);
}

Value *
builtin_set_car (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 2)
    return val_error ("set-car!: expects exactly two argument");

  Value *list = evaluate_expression (environment, CAR (arguments));
  Value *value = evaluate_expression (environment, CADR (arguments));

  if (list->type != VALUE_CONS)
    return val_error ("set-car!: first argument is not a pair");

  CAR (list) = value;

  return list;
}

Value *
builtin_set_cdr (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 2)
    return val_error ("set-cdr!: expects exactly two argument");

  Value *list = evaluate_expression (environment, CAR (arguments));
  Value *value = evaluate_expression (environment, CADR (arguments));

  if (list->type != VALUE_CONS)
    return val_error ("set-cdr!: first argument is not a pair");

  CDR (list) = value;

  return list;
}

Value *
builtin_length (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("length: expects exactly one argument");

  Value *list = evaluate_expression (environment, CAR (arguments));

  int count = 0;
  while (list->type == VALUE_CONS)
    {
      count++;
      list = CDR (list);
    };

  return val_integer (count);
}

Value *
builtin_reverse (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("reverse: expects exactly one argument");

  Value *list = evaluate_expression (environment, CAR (arguments));
  Value *result = val_nil ();

  while (list->type == VALUE_CONS)
    {
      result = val_cons (CAR (list), result);
      list = CDR (list);
    }

  return result;
}

Value *
builtin_filter (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 2)
    return val_error ("filter: expects exactly two arguments");

  Value *function = evaluate_expression (environment, CAR (arguments));

  if (function->type != VALUE_BUILTIN && function->type != VALUE_LAMBDA)
    return val_error ("filter: first argument must be a function");

  Value *list = evaluate_expression (environment, CADR (arguments));
  if (list->type != VALUE_CONS)
    return val_error ("filter: second argument should be list/cons");

  Value *result_head = val_cons (val_nil (), val_nil ());
  Value *result_tail = result_head;

  while (list->type == VALUE_CONS && !IS_NULL (list))
    {
      Value *current_element = CAR (list);
      Value *function_arguments = val_cons (current_element, val_nil ());
      Value *predicate_result
          = apply (environment, function, function_arguments);
      ERROR_OUT (predicate_result);

      if (!IS_NULL (predicate_result))
        {
          Value *new_cell = val_cons (current_element, val_nil ());

          CDR (result_tail) = new_cell;
          result_tail = new_cell;
        }

      list = CDR (list);
    }

  return CDR (result_head);
}
