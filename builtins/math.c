#include "builtins/math.h"

#include <math.h>

#include "core/value.h"
#include "core/eval.h"

static Val *
get_numeric_value (Val *node, double *out, int *is_float)
{
  *is_float = 0;

  if (!node)
    return make_error ("numeric operation got null node");

  switch (node->type)
    {
    case VALUE_INTEGER:
      *out = (double)node->as.INTEGER;
      return nil ();
    case VALUE_FLOAT:
      *out = node->as.FLOAT;
      *is_float = 1;
      return nil ();
    default:
      return make_error ("numeric operation expects number");
    }
}

Val *
builtin_add (Val *env, Val *args)
{
  double result = 0.0;
  int has_float = 0;

  while (args->type == VALUE_CONS)
    {
      int is_float = 0;
      Val *val_node = evaluate_expression (env, CAR (args));
      ERROR_OUT (val_node);

      double value;
      Val *err = get_numeric_value (val_node, &value, &is_float);
      ERROR_OUT (err);

      if (is_float)
        has_float = 1;
      result += value;

      args = CDR (args);
    }

  return has_float ? make_float (result) : make_integer ((long)result);
}

Val *
builtin_sub (Val *env, Val *args)
{
  if (IS_NULL (args))
    return make_error ("- expects at levalue one argument");

  int is_float = 0;
  Val *first_node = evaluate_expression (env, CAR (args));
  ERROR_OUT (first_node);

  double result;
  Val *err = get_numeric_value (first_node, &result, &is_float);
  ERROR_OUT (err);

  args = CDR (args);

  if (IS_NULL (args))
    return is_float ? make_float (-result) : make_integer ((long)-result);

  int has_float = is_float;

  while (args->type == VALUE_CONS)
    {
      int arg_float = 0;
      Val *val_node = evaluate_expression (env, CAR (args));
      ERROR_OUT (val_node);

      double value;
      err = get_numeric_value (val_node, &value, &arg_float);
      ERROR_OUT (err);

      if (arg_float)
        has_float = 1;
      result -= value;

      args = CDR (args);
    }

  return has_float ? make_float (result) : make_integer ((long)result);
}

Val *
builtin_mul (Val *env, Val *args)
{
  double result = 1.0;
  int has_float = 0;

  while (args->type == VALUE_CONS)
    {
      int is_float = 0;
      Val *val_node = evaluate_expression (env, CAR (args));
      ERROR_OUT (val_node);

      double value;
      Val *err = get_numeric_value (val_node, &value, &is_float);
      ERROR_OUT (err);

      if (is_float)
        has_float = 1;
      result *= value;

      args = CDR (args);
    }

  return has_float ? make_float (result) : make_integer ((long)result);
}

Val *
builtin_div (Val *env, Val *args)
{
  if (IS_NULL (args))
    return make_error ("/ expects at levalue one argument");

  int is_float = 0;
  Val *first_node = evaluate_expression (env, CAR (args));
  ERROR_OUT (first_node);

  double result;
  Val *err = get_numeric_value (first_node, &result, &is_float);
  ERROR_OUT (err);

  args = CDR (args);

  if (IS_NULL (args))
    return make_float (1.0 / result);

  while (args->type == VALUE_CONS)
    {
      int arg_float = 0;
      Val *val_node = evaluate_expression (env, CAR (args));
      ERROR_OUT (val_node);

      double divisor;
      err = get_numeric_value (val_node, &divisor, &arg_float);
      ERROR_OUT (err);

      if (divisor == 0.0)
        return make_error ("division by zero");

      result /= divisor;
      args = CDR (args);
    }

  return make_float (result);
}

Val *
builtin_mod (Val *env, Val *args)
{
  if (arguments_length (args) != 2)
    return make_error ("mod expects exactly two arguments");

  int is_float_a = 0, is_float_b = 0;
  double a, b;

  Val *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Val *err = get_numeric_value (node_a, &a, &is_float_a);
  ERROR_OUT (err);

  Val *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &is_float_b);
  ERROR_OUT (err);

  if (b == 0.0)
    return make_error ("mod: division by zero");

  return make_integer ((long)fmod (a, b));
}

Val *
builtin_expt (Val *env, Val *args)
{
  if (arguments_length (args) != 2)
    return make_error ("expt expects exactly two arguments");

  int is_float_a = 0, is_float_b = 0;
  double a, b;

  Val *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Val *err = get_numeric_value (node_a, &a, &is_float_a);
  ERROR_OUT (err);

  Val *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &is_float_b);
  ERROR_OUT (err);

  double result = pow (a, b);

  if (!is_float_a && !is_float_b && b >= 0 && b == (long)b)
    return make_integer ((long)result);
  else
    return make_float (result);
}

Val *
builtin_abs (Val *env, Val *args)
{
  if (arguments_length (args) != 1)
    return make_error ("abs expects exactly one argument");

  int is_float = 0;
  double a;
  Val *node = evaluate_expression (env, CAR (args));
  ERROR_OUT (node);

  Val *err = get_numeric_value (node, &a, &is_float);
  ERROR_OUT (err);

  return is_float ? make_float (fabs (a)) : make_integer ((long)fabs (a));
}

Val *
builtin_sqrt (Val *env, Val *args)
{
  if (arguments_length (args) != 1)
    return make_error ("sqrt expects exactly one argument");

  int is_float = 0;
  double a;
  Val *node = evaluate_expression (env, CAR (args));
  ERROR_OUT (node);

  Val *err = get_numeric_value (node, &a, &is_float);
  ERROR_OUT (err);

  if (a < 0.0)
    return make_error ("sqrt: negative number");

  return make_float (sqrt (a));
}

Val *
builtin_num_eq (Val *env, Val *args)
{
  if (arguments_length (args) != 2)
    return make_error ("= expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Val *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Val *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Val *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a == b) ? t () : nil ();
}

Val *
builtin_num_gt (Val *env, Val *args)
{
  if (arguments_length (args) != 2)
    return make_error ("> expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Val *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Val *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Val *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a > b) ? t () : nil ();
}

Val *
builtin_num_lt (Val *env, Val *args)
{
  if (arguments_length (args) != 2)
    return make_error ("< expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Val *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Val *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Val *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a < b) ? t () : nil ();
}

Val *
builtin_num_gte (Val *env, Val *args)
{
  if (arguments_length (args) != 2)
    return make_error (">= expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Val *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Val *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Val *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a >= b) ? t () : nil ();
}

Val *
builtin_num_lte (Val *env, Val *args)
{
  if (arguments_length (args) != 2)
    return make_error ("<= expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Val *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Val *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Val *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a <= b) ? t () : nil ();
}

Val *
builtin_floor (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("floor: expects exactly one arguments");

  Val *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  Val *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return make_integer (floor (a));
}

Val *
builtin_ceil (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("ceil: expects exactly one arguments");

  Val *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  Val *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return make_integer (ceil (a));
}

Val *
builtin_round (Val *environment, Val *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("round: expects exactly one arguments");

  Val *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  Val *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return make_integer (round (a));
}
