#include "core/quasiquote.h"
#include "core/value.h"

static const Meta QQ_EXPAND_META
    = { .filename = "<quasiquote-expanded>", .line_number = 0 };

static int
is_symbol_named (Value *node, const char *name)
{
  return node->type == VALUE_SYMBOL && strcmp (node->as.SYMBOL, name) == 0;
}

static int
is_unquote (Value *node)
{
  return node->type == VALUE_CONS && is_symbol_named (CAR (node), "unquote")
         && !IS_NULL (CDR (node)) && IS_NULL (CDDR (node));
}

static int
is_unquote_splicing (Value *node)
{
  return node->type == VALUE_CONS
         && is_symbol_named (CAR (node), "unquote-splicing")
         && !IS_NULL (CDR (node)) && IS_NULL (CDDR (node));
}

Value *
expand_quasiquote (Value *expr, int depth)
{
  if (expr->type != VALUE_CONS)
    {
      // Basic types (integers, strings, or symbols) get quoted
      return val_cons (val_symbol ("quote", QQ_EXPAND_META),
                       val_cons (expr, val_nil ()));
    }

  // Handle ,expression
  if (is_unquote (expr))
    {
      if (depth == 1)
        return CADR (expr);
      // If nested, we stay in quasiquote mode
      return val_cons (
          val_symbol ("list", QQ_EXPAND_META),
          val_cons (val_cons (val_symbol ("quote", QQ_EXPAND_META),
                              val_cons (val_symbol ("unquote", QQ_EXPAND_META),
                                        val_nil ())),
                    val_cons (expand_quasiquote (CADR (expr), depth - 1),
                              val_nil ())));
    }

  // Handle ( ,@splice . rest )
  Value *head = CAR (expr);
  if (is_unquote_splicing (head) && depth == 1)
    {
      // This turns `(,@a . b) into (append a `b)
      return val_cons (val_symbol ("append", QQ_EXPAND_META),
                       val_cons (CADR (head), val_cons (expand_quasiquote (
                                                            CDR (expr), depth),
                                                        val_nil ())));
    }

  // Handle standard ( head . rest )
  // This turns `(a . b) into (cons `a `b)
  return val_cons (
      val_symbol ("cons", QQ_EXPAND_META),
      val_cons (expand_quasiquote (head, depth),
                val_cons (expand_quasiquote (CDR (expr), depth), val_nil ())));
}
