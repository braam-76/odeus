#include "builtins/math.h"
#include "core/ast.h"
#include "core/eval.h"
#include <math.h>

/* Evaluate node and extract numeric value (integer or float) */
static AST *
get_numeric_value (AST *node, double *out, int *is_float)
{
  if (node->type == AST_INTEGER)
    {
      *out = (double)node->as.INTEGER;
      return nil ();
    }

  if (node->type == AST_FLOAT)
    {
      *out = node->as.FLOAT;
      *is_float = 1;
      return nil ();
    }

  return make_error ("numeric operation expects number");
}

AST *
builtin_add (AST *environment, AST *arguments)
{
  double result = 0.0;
  int is_float = 0;

  while (arguments->type == AST_CONS)
    {
      AST *arg = CAR (arguments);

      AST *value_node = evaluate_expression (environment, arg);
      ERROR_OUT (value_node);

      double value;
      AST *err = get_numeric_value (value_node, &value, &is_float);
      ERROR_OUT (err);

      result += value;
      arguments = CDR (arguments);
    }

  return is_float ? make_float (result) : make_integer ((long)result);
}

AST *
builtin_sub (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("- expects at least one argument");

  int is_float = 0;

  /* Evaluate first argument */
  AST *first_node = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (first_node);

  double result;
  AST *err = get_numeric_value (first_node, &result, &is_float);
  ERROR_OUT (err);

  arguments = CDR (arguments);

  /* Unary case: negate the first argument */
  if (IS_NULL (arguments))
    {
      result = -result;
      return is_float ? make_float (result) : make_integer ((long)result);
    }

  /* N-ary subtraction */
  while (arguments->type == AST_CONS)
    {
      AST *arg_node = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (arg_node);

      double value;
      err = get_numeric_value (arg_node, &value, &is_float);
      ERROR_OUT (err);

      result -= value;
      arguments = CDR (arguments);
    }

  return is_float ? make_float (result) : make_integer ((long)result);
}

AST *
builtin_mul (AST *environment, AST *arguments)
{
  double result = 1.0;
  int is_float = 0;

  while (arguments->type == AST_CONS)
    {
      AST *arg = CAR (arguments);

      AST *value_node = evaluate_expression (environment, arg);
      ERROR_OUT (value_node);

      double value;
      AST *err = get_numeric_value (value_node, &value, &is_float);
      ERROR_OUT (err);

      result *= value;
      arguments = CDR (arguments);
    }

  return is_float ? make_float (result) : make_integer ((long)result);
}

AST *
builtin_div (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("/ expects at least one argument");

  int is_float = 0;

  AST *first_node = evaluate_expression (environment, CAR (arguments));
  ERROR_OUT (first_node);

  double result;
  AST *err = get_numeric_value (first_node, &result, &is_float);
  ERROR_OUT (err);

  arguments = CDR (arguments);

  while (arguments->type == AST_CONS)
    {
      AST *arg_node = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (arg_node);

      double divisor;
      err = get_numeric_value (arg_node, &divisor, &is_float);
      ERROR_OUT (err);

      if (divisor == 0.0)
        return make_error ("division by zero");

      result /= divisor;
      arguments = CDR (arguments);
    }

  /* Division always produces float */
  return make_float (result);
}

//
// More math
//

AST *
builtin_mod (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("mod: expects exactly two arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));
  AST *second_node = evaluate_expression (environment, CADR (arguments));

  double a, b;
  int is_float_a, is_float_b;

  AST *err = get_numeric_value (first_node, &a, &is_float_a);
  ERROR_OUT (err);

  err = get_numeric_value (second_node, &b, &is_float_b);
  ERROR_OUT (err);

  if (b == 0.0)
    return make_error ("mod: division by zero");

  return make_float (fmod (a, b));
}

AST *
builtin_expt (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("expt: expects exactly two arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));
  AST *second_node = evaluate_expression (environment, CADR (arguments));

  double a, b;
  int is_float_a, is_float_b;

  AST *err = get_numeric_value (first_node, &a, &is_float_a);
  ERROR_OUT (err);

  err = get_numeric_value (second_node, &b, &is_float_b);
  ERROR_OUT (err);

  double result = pow (a, b);

  // expt with integer exponents can return integer
  // Check if both are integers and exponent is non-negative
  if (!is_float_a && !is_float_b && b >= 0 && b == (long)b)
    return make_integer ((long)pow (a, b));
  else
    return make_float (result);
}

AST *
builtin_sqrt (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("sqrt: expects exactly one arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  AST *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  if (a < 0)
    return make_error ("sqrt: can't take square root of negative number");

  return make_float (sqrt (a));
}

AST *
builtin_abs (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("abs: expects exactly one arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  AST *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  double result = fabs (a);

  if (is_float)
    return make_float (result);
  else
    return make_integer ((long)result);
}

AST *
builtin_floor (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("floor: expects exactly one arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  AST *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return make_integer (floor (a));
}

AST *
builtin_ceil (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("ceil: expects exactly one arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  AST *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return make_integer (ceil (a));
}

AST *
builtin_round (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 1)
    return make_error ("round: expects exactly one arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));

  double a;
  int is_float;

  AST *err = get_numeric_value (first_node, &a, &is_float);
  ERROR_OUT (err);

  return make_integer (round (a));
}

//
// Numeric comparison (fixed with get_numeric_value)
//

AST *
builtin_num_eq (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("= (numeric equal): expects exactly two arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));
  AST *second_node = evaluate_expression (environment, CADR (arguments));

  double a, b;
  int unused;

  AST *err = get_numeric_value (first_node, &a, &unused);
  ERROR_OUT (err);

  err = get_numeric_value (second_node, &b, &unused);
  ERROR_OUT (err);

  return (a == b) ? t () : nil ();
}

AST *
builtin_num_gt (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("> (greater than): expects exactly two arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));
  AST *second_node = evaluate_expression (environment, CADR (arguments));

  double a, b;
  int unused;

  AST *err = get_numeric_value (first_node, &a, &unused);
  ERROR_OUT (err);

  err = get_numeric_value (second_node, &b, &unused);
  ERROR_OUT (err);

  return (a > b) ? t () : nil ();
}

AST *
builtin_num_lt (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("< (less than): expects exactly two arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));
  AST *second_node = evaluate_expression (environment, CADR (arguments));

  double a, b;
  int unused;

  AST *err = get_numeric_value (first_node, &a, &unused);
  ERROR_OUT (err);

  err = get_numeric_value (second_node, &b, &unused);
  ERROR_OUT (err);

  return (a < b) ? t () : nil ();
}

AST *
builtin_num_gte (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error (">= (greater than or equals to): expects exactly two arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));
  AST *second_node = evaluate_expression (environment, CADR (arguments));

  double a, b;
  int unused;

  AST *err = get_numeric_value (first_node, &a, &unused);
  ERROR_OUT (err);

  err = get_numeric_value (second_node, &b, &unused);
  ERROR_OUT (err);

  return (a >= b) ? t () : nil ();
}

AST *
builtin_num_lte (AST *environment, AST *arguments)
{
  if (arguments_length (arguments) != 2)
    return make_error ("<= (less than or equals to): expects exactly two arguments");

  AST *first_node = evaluate_expression (environment, CAR (arguments));
  AST *second_node = evaluate_expression (environment, CADR (arguments));

  double a, b;
  int unused;

  AST *err = get_numeric_value (first_node, &a, &unused);
  ERROR_OUT (err);

  err = get_numeric_value (second_node, &b, &unused);
  ERROR_OUT (err);

  return (a <= b) ? t () : nil ();
}
