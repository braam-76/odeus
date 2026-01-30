#include "core/quasiquote.h"
#include "core/value.h"

static int
is_symbol_named (Val *node, const char *name)
{
  return node->type == VALUE_SYMBOL && strcmp (node->as.SYMBOL, name) == 0;
}

static int
is_unquote (Val *node)
{
  return node->type == VALUE_CONS && is_symbol_named (CAR (node), "unquote")
         && !IS_NULL (CDR (node)) && IS_NULL (CDDR (node));
}

static int
is_unquote_splicing (Val *node)
{
  return node->type == VALUE_CONS
         && is_symbol_named (CAR (node), "unquote-splicing")
         && !IS_NULL (CDR (node)) && IS_NULL (CDDR (node));
}

Val *
expand_quasiquote (Val *expr, int depth)
{
  if (expr->type != VALUE_CONS)
    {
      // Basic types (integers, strings, or symbols) get quoted
      return make_cons (make_symbol ("quote"), make_cons (expr, nil ()));
    }

  // Handle ,expression
  if (is_unquote (expr))
    {
      if (depth == 1)
        return CADR (expr);
      // If nested, we stay in quasiquote mode
      return make_cons (
          make_symbol ("list"),
          make_cons (
              make_cons (make_symbol ("quote"),
                         make_cons (make_symbol ("unquote"), nil ())),
              make_cons (expand_quasiquote (CADR (expr), depth - 1), nil ())));
    }

  // Handle ( ,@splice . rest )
  Val *head = CAR (expr);
  if (is_unquote_splicing (head) && depth == 1)
    {
      // This turns `(,@a . b) into (append a `b)
      return make_cons (
          make_symbol ("append"),
          make_cons (
              CADR (head),
              make_cons (expand_quasiquote (CDR (expr), depth), nil ())));
    }

  // Handle standard ( head . rest )
  // This turns `(a . b) into (cons `a `b)
  return make_cons (
      make_symbol ("cons"),
      make_cons (expand_quasiquote (head, depth),
                 make_cons (expand_quasiquote (CDR (expr), depth), nil ())));
}
