#include "builtins/math.h"

#include <math.h>

#include "core/value.h"
#include "core/eval.h"

static Value *
get_numeric_value (Value *node, double *out, int *is_float)
{
  *is_float = 0;

  if (!node)
    return val_error ("numeric operation got null node");

  switch (node->type)
    {
    case VALUE_INTEGER:
      *out = (double)node->as.INTEGER;
      return val_nil ();
    case VALUE_FLOAT:
      *out = node->as.FLOAT;
      *is_float = 1;
      return val_nil ();
    default:
      return val_error ("numeric operation expects number");
    }
}

Value *
builtin_add (Environment *env, Value *args)
{
  double result = 0.0;
  int has_float = 0;

  while (args->type == VALUE_CONS)
    {
      int is_float = 0;
      Value *val_node = evaluate_expression (env, CAR (args));
      ERROR_OUT (val_node);

      double value;
      Value *err = get_numeric_value (val_node, &value, &is_float);
      ERROR_OUT (err);

      if (is_float)
        has_float = 1;
      result += value;

      args = CDR (args);
    }

  return has_float ? val_float (result) : val_integer ((long)result);
}

Value *
builtin_sub (Environment *env, Value *args)
{
  if (IS_NULL (args))
    return val_error ("- expects at levalue one argument");

  int is_float = 0;
  Value *first_node = evaluate_expression (env, CAR (args));
  ERROR_OUT (first_node);

  double result;
  Value *err = get_numeric_value (first_node, &result, &is_float);
  ERROR_OUT (err);

  args = CDR (args);

  if (IS_NULL (args))
    return is_float ? val_float (-result) : val_integer ((long)-result);

  int has_float = is_float;

  while (args->type == VALUE_CONS)
    {
      int arg_float = 0;
      Value *val_node = evaluate_expression (env, CAR (args));
      ERROR_OUT (val_node);

      double value;
      err = get_numeric_value (val_node, &value, &arg_float);
      ERROR_OUT (err);

      if (arg_float)
        has_float = 1;
      result -= value;

      args = CDR (args);
    }

  return has_float ? val_float (result) : val_integer ((long)result);
}

Value *
builtin_mul (Environment *env, Value *args)
{
  double result = 1.0;
  int has_float = 0;

  while (args->type == VALUE_CONS)
    {
      int is_float = 0;
      Value *val_node = evaluate_expression (env, CAR (args));
      ERROR_OUT (val_node);

      double value;
      Value *err = get_numeric_value (val_node, &value, &is_float);
      ERROR_OUT (err);

      if (is_float)
        has_float = 1;
      result *= value;

      args = CDR (args);
    }

  return has_float ? val_float (result) : val_integer ((long)result);
}

Value *
builtin_div (Environment *env, Value *args)
{
  if (IS_NULL (args))
    return val_error ("/ expects at levalue one argument");

  int is_float = 0;
  Value *first_node = evaluate_expression (env, CAR (args));
  ERROR_OUT (first_node);

  double result;
  Value *err = get_numeric_value (first_node, &result, &is_float);
  ERROR_OUT (err);

  args = CDR (args);

  if (IS_NULL (args))
    return val_float (1.0 / result);

  while (args->type == VALUE_CONS)
    {
      int arg_float = 0;
      Value *val_node = evaluate_expression (env, CAR (args));
      ERROR_OUT (val_node);

      double divisor;
      err = get_numeric_value (val_node, &divisor, &arg_float);
      ERROR_OUT (err);

      if (divisor == 0.0)
        return val_error ("division by zero");

      result /= divisor;
      args = CDR (args);
    }

  return val_float (result);
}

Value *
builtin_mod (Environment *env, Value *args)
{
  if (arguments_length (args) != 2)
    return val_error ("mod expects exactly two arguments");

  int is_float_a = 0, is_float_b = 0;
  double a, b;

  Value *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Value *err = get_numeric_value (node_a, &a, &is_float_a);
  ERROR_OUT (err);

  Value *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &is_float_b);
  ERROR_OUT (err);

  if (b == 0.0)
    return val_error ("mod: division by zero");

  return val_integer ((long)fmod (a, b));
}

Value *
builtin_expt (Environment *env, Value *args)
{
  if (arguments_length (args) != 2)
    return val_error ("expt expects exactly two arguments");

  int is_float_a = 0, is_float_b = 0;
  double a, b;

  Value *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Value *err = get_numeric_value (node_a, &a, &is_float_a);
  ERROR_OUT (err);

  Value *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &is_float_b);
  ERROR_OUT (err);

  double result = pow (a, b);

  if (!is_float_a && !is_float_b && b >= 0 && b == (long)b)
    return val_integer ((long)result);
  else
    return val_float (result);
}

Value *
builtin_abs (Environment *env, Value *args)
{
  if (arguments_length (args) != 1)
    return val_error ("abs expects exactly one argument");

  int is_float = 0;
  double a;
  Value *node = evaluate_expression (env, CAR (args));
  ERROR_OUT (node);

  Value *err = get_numeric_value (node, &a, &is_float);
  ERROR_OUT (err);

  return is_float ? val_float (fabs (a)) : val_integer ((long)fabs (a));
}

Value *
builtin_sqrt (Environment *env, Value *args)
{
  if (arguments_length (args) != 1)
    return val_error ("sqrt expects exactly one argument");

  int is_float = 0;
  double a;
  Value *node = evaluate_expression (env, CAR (args));
  ERROR_OUT (node);

  Value *err = get_numeric_value (node, &a, &is_float);
  ERROR_OUT (err);

  if (a < 0.0)
    return val_error ("sqrt: negative number");

  return val_float (sqrt (a));
}

Value *
builtin_num_eq (Environment *env, Value *args)
{
  if (arguments_length (args) != 2)
    return val_error ("= expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Value *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Value *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Value *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a == b) ? val_t () : val_nil ();
}

Value *
builtin_num_gt (Environment *env, Value *args)
{
  if (arguments_length (args) != 2)
    return val_error ("> expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Value *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Value *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Value *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a > b) ? val_t () : val_nil ();
}

Value *
builtin_num_lt (Environment *env, Value *args)
{
  if (arguments_length (args) != 2)
    return val_error ("< expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Value *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Value *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Value *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a < b) ? val_t () : val_nil ();
}

Value *
builtin_num_gte (Environment *env, Value *args)
{
  if (arguments_length (args) != 2)
    return val_error (">= expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Value *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Value *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Value *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a >= b) ? val_t () : val_nil ();
}

Value *
builtin_num_lte (Environment *env, Value *args)
{
  if (arguments_length (args) != 2)
    return val_error ("<= expects exactly two arguments");

  int unused1 = 0, unused2 = 0;
  double a, b;

  Value *node_a = evaluate_expression (env, CAR (args));
  ERROR_OUT (node_a);
  Value *err = get_numeric_value (node_a, &a, &unused1);
  ERROR_OUT (err);

  Value *node_b = evaluate_expression (env, CADR (args));
  ERROR_OUT (node_b);
  err = get_numeric_value (node_b, &b, &unused2);
  ERROR_OUT (err);

  return (a <= b) ? val_t () : val_nil ();
}

Value *
builtin_floor (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("floor: expects exactly one arguments");

  Value *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  Value *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return val_integer (floor (a));
}

Value *
builtin_ceil (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("ceil: expects exactly one arguments");

  Value *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  Value *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return val_integer (ceil (a));
}

Value *
builtin_round (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("round: expects exactly one arguments");

  Value *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  Value *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return val_integer (round (a));
}
