#include "core/quasiquote.h"
#include "core/ast.h"

static AST *expand_quasiquote_list (AST *list, int depth);

static int
is_symbol_named (AST *node, const char *name)
{
  return node->type == AST_SYMBOL && strcmp (node->as.SYMBOL, name) == 0;
}

static int
is_unquote (AST *node)
{
  return node->type == AST_CONS && is_symbol_named (CAR (node), "unquote")
         && !IS_NULL (CDR (node)) && IS_NULL (CDDR (node));
}

static int
is_unquote_splicing (AST *node)
{
  return node->type == AST_CONS
         && is_symbol_named (CAR (node), "unquote-splicing")
         && !IS_NULL (CDR (node)) && IS_NULL (CDDR (node));
}

AST *
expand_quasiquote (AST *expr, int depth)
{
  if (!expr)
    return nil ();

  if (expr->type != AST_CONS)
    return (depth == 1)
               ? make_cons (make_symbol ("quote"), make_cons (expr, nil ()))
               : expr;

  if (is_unquote (expr))
    {
      if (depth == 1)
        return CADR (expr);
      else
        return make_cons (
            make_cons (make_symbol ("quote"),
                       make_cons (make_symbol ("unquote"), nil ())),
            make_cons (expand_quasiquote (CADR (expr), depth - 1), nil ()));
    }

  if (is_unquote_splicing (expr) && depth == 1)
    return make_error ("unquote-splicing not in list context");

  if (is_symbol_named (CAR (expr), "quasiquote"))
    {
      if (depth == 1)
        return make_cons (
            make_symbol ("quasiquote"),
            make_cons (expand_quasiquote (CADR (expr), depth + 1), nil ()));
      else
        return make_cons (
            make_cons (make_symbol ("quote"),
                       make_cons (make_symbol ("quasiquote"), nil ())),
            make_cons (expand_quasiquote (CADR (expr), depth + 1), nil ()));
    }

  return expand_quasiquote_list (expr, depth);
}

static AST *
expand_quasiquote_list (AST *list, int depth)
{
  if (IS_NULL (list))
    return make_cons (make_symbol ("quote"), make_cons (nil (), nil ()));

  AST *elements = nil ();
  AST *last = NULL;
  AST *current = list;

  while (current->type == AST_CONS)
    {
      AST *head = CAR (current);

      if (is_unquote_splicing (head) && depth == 1)
        {
          AST *splice_expression = CADR (head);
          AST *rest = expand_quasiquote_list (CDR (current), depth);

          // Wrap elements before the splice
          AST *list_before = IS_NULL (elements)
                                 ? nil ()
                                 : make_cons (make_symbol ("list"), elements);

          // Build append: (append <list-before> <splice-expr> <rest>)
          // The splice_expression is already a list, so use it directly
          AST *append_expression = NULL;

          if (IS_NULL (list_before))
            append_expression = make_cons (
                make_symbol ("append"),
                make_cons (splice_expression,
                           make_cons (rest, nil ())));
          else
            append_expression = make_cons (
                make_symbol ("append"),
                make_cons (list_before,
                           make_cons (splice_expression,
                                      make_cons (rest, nil ()))));

          return append_expression;
        }

      AST *expanded = expand_quasiquote (head, depth);
      AST *cell = make_cons (expanded, nil ());

      if (IS_NULL (elements))
        elements = cell;
      else
        last->as.CONS.CDR = cell;

      last = cell;
      current = CDR (current);
    }

  // no splices found, just wrap elements in list
  return make_cons (make_symbol ("list"), elements);
}
