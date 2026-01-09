#include "basic_math.h"

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
  (void)environment;
  double result = 0.0;
  int is_float = 0;

  while (arguments->type == AST_CONS)
    {
      double value;
      AST *err = get_numeric_value (CAR (arguments), &value, &is_float);
      if (err->type == AST_ERROR)
        return err;

      result += value;
      arguments = CDR (arguments);
    }

  return is_float ? make_float (result) : make_integer ((long)result);
}

AST *
builtin_sub (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments))
    return make_error ("- expects at least one argument");

  int is_float = 0;
  double result;

  AST *err = get_numeric_value (CAR (arguments), &result, &is_float);
  if (err->type == AST_ERROR)
    return err;

  arguments = CDR (arguments);

  while (arguments->type == AST_CONS)
    {
      double value;
      err = get_numeric_value (CAR (arguments), &value, &is_float);
      if (err->type == AST_ERROR)
        return err;

      result -= value;
      arguments = CDR (arguments);
    }

  return is_float ? make_float (result) : make_integer ((long)result);
}

AST *
builtin_mul (AST *environment, AST *arguments)
{
  (void)environment;
  double result = 1.0;
  int is_float = 0;

  while (arguments->type == AST_CONS)
    {
      double value;
      AST *err = get_numeric_value (CAR (arguments), &value, &is_float);
      if (err->type == AST_ERROR)
        return err;

      result *= value;
      arguments = CDR (arguments);
    }

  return is_float ? make_float (result) : make_integer ((long)result);
}

AST *
builtin_div (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments))
    return make_error ("/ expects at least one argument");

  int is_float = 0;
  double result;

  AST *err = get_numeric_value (CAR (arguments), &result, &is_float);
  if (err->type == AST_ERROR)
    return err;

  arguments = CDR (arguments);

  while (arguments->type == AST_CONS)
    {
      double divisor;
      err = get_numeric_value (CAR (arguments), &divisor, &is_float);
      if (err->type == AST_ERROR)
        return err;

      if (divisor == 0.0)
        return make_error ("division by zero");

      result /= divisor;
      arguments = CDR (arguments);
    }

  /* Division always produces float */
  return make_float (result);
}
