#include "builtins/forms.h"
#include "core/ast.h"

static AST *expand_quasiquote (AST *expr, int depth);
static AST *qq_symbol (const char *name, int depth);

static int
is_reserved_symbol (const char *name)
{
  return strcmp (name, "unquote") == 0 || strcmp (name, "unquote-splicing") == 0
         || strcmp (name, "quasiquote") == 0;
}

AST *
builtin_define (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments) || IS_NULL (CDR (arguments)))
    return make_error ("ERROR: define expects (symbol value_expression)\n");

  AST *symbol = CAR (arguments);
  if (symbol->type != AST_SYMBOL)
    return make_error ("ERROR: define: first argument must be symbol\n");

  if (is_reserved_symbol (symbol->as.SYMBOL))
    return make_error ("ERROR: cannot redefine reserved syntax\n");

  AST *value = evaluate_expression (environment, CADR (arguments));
  ERROR_OUT (value);

  environment_set (environment, symbol, value);
  return symbol;
}

AST *
builtin_begin (AST *environment, AST *arguments)
{
  AST *last_value = nil ();

  while (arguments->type == AST_CONS)
    {
      last_value = evaluate_expression (environment, CAR (arguments));
      ERROR_OUT (last_value);
      arguments = CDR (arguments);
    }

  return last_value;
}

AST *
builtin_eval (AST *env, AST *args)
{
  if (IS_NULL (args) || !IS_NULL (CDR (args)))
    return make_error ("eval expects exactly one argument");

  AST *expr = CAR (args);

  AST *expr_to_eval = evaluate_expression (env, expr);
  ERROR_OUT (expr_to_eval);

  // If it's a quote node, unwrap it
  if (expr_to_eval->type == AST_QUOTE)
    expr_to_eval = expr_to_eval->as.QUOTE.EXPR;

  return evaluate_expression (env, expr_to_eval);
}

AST *
builtin_quote (AST *environment, AST *arguments)
{
  (void)environment;

  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("ERROR: quote expects one argument\n");

  return CAR (arguments);
}

AST *
builtin_quasiquote (AST *environment, AST *arguments)
{
  (void)environment;
  if (IS_NULL (arguments) || !IS_NULL (CDR (arguments)))
    return make_error ("quasiquote expects exactly one argument");

  AST *res = expand_quasiquote (CAR (arguments), 1);

  printf ("Quasiquote expanded to: ");
  ast_print (res);
  printf ("\n");

  return res;
}

AST *
builtin_lambda (AST *environment, AST *arguments)
{
  if (IS_NULL (arguments))
    return make_error ("ERROR: lambda expects at least parameter list\n");

  AST *parameters = CAR (arguments);
  AST *body = CDR (arguments);

  AST *lambda = malloc (sizeof (AST));
  lambda->type = AST_LAMBDA;
  lambda->as.LAMBDA.parameters = parameters;
  lambda->as.LAMBDA.body = body;
  lambda->as.LAMBDA.environment = environment;

  return lambda;
}

//
// Quasiquote helpers
//

static AST *expand_quasiquote_list (AST *list, int depth);

static int is_symbol_named (AST *node, const char *name);
static int is_unquote (AST *node);
static int is_unquote_splicing (AST *node);

static AST *
expand_quasiquote (AST *expr, int depth)
{
  if (!expr)
    return nil ();

  /* atoms evaluate to themselves */
  if (expr->type != AST_CONS)
    return expr;

  /* (unquote x) */
  if (is_unquote (expr))
    {
      if (depth == 1)
        return CADR (expr); /* splice value */
      else
        return make_cons (make_quote (make_symbol ("unquote")),
                          make_cons (expand_quasiquote (CADR (expr), depth - 1), nil ()));
    }

  /* illegal splice outside list at active depth */
  if (is_unquote_splicing (expr) && depth == 1)
    return make_error ("unquote-splicing not in list context");

  /* nested quasiquote */
  if (is_symbol_named (CAR (expr), "quasiquote"))
    {
      if (depth == 1)
        return make_cons (make_symbol ("quasiquote"),
                          make_cons (expand_quasiquote (CADR (expr), depth + 1), nil ()));
      else
        return make_cons (make_quote (make_symbol ("quasiquote")),
                          make_cons (expand_quasiquote (CADR (expr), depth + 1), nil ()));
    }

  /* general list */
  return expand_quasiquote_list (expr, depth);
}

static AST *
expand_quasiquote_list (AST *list, int depth)
{
  /* empty list => '() */
  if (IS_NULL (list))
    return make_quote (nil ());

  AST *before = nil ();
  AST *last = NULL;
  AST *curr = list;

  while (curr->type == AST_CONS)
    {
      AST *head = CAR (curr);

      /* handle (unquote-splicing x) */
      if (is_unquote_splicing (head) && depth == 1)
        {
          AST *splice = CADR (head);
          AST *rest = expand_quasiquote_list (CDR (curr), depth);

          if (IS_NULL (before))
            return make_cons (qq_symbol ("append", depth),
                              make_cons (splice, make_cons (rest, nil ())));

          AST *list_expr = make_cons (qq_symbol ("list", depth), before);

          return make_cons (
              qq_symbol ("append", depth),
              make_cons (list_expr,
                         make_cons (make_cons (qq_symbol ("append", depth),
                                               make_cons (splice, make_cons (rest, nil ()))),
                                    nil ())));
        }

      /* normal element */
      AST *expanded = expand_quasiquote (head, depth);
      AST *cell = make_cons (expanded, nil ());

      if (IS_NULL (before))
        before = cell;
      else
        last->as.CONS.CDR = cell;

      last = cell;
      curr = CDR (curr);
    }

  /* no splicing encountered */
  return make_cons (qq_symbol ("list", depth), before);
}

static AST *
qq_symbol (const char *name, int depth)
{
  AST *sym = make_symbol (name);
  return (depth == 1) ? sym : make_quote (sym);
}

int
is_symbol_named (AST *node, const char *name)
{
  return node->type == AST_SYMBOL && strcmp (node->as.SYMBOL, name) == 0;
}

int
is_unquote (AST *node)
{
  return node->type == AST_CONS && is_symbol_named (CAR (node), "unquote");
}

int
is_unquote_splicing (AST *node)
{
  return node->type == AST_CONS && is_symbol_named (CAR (node), "unquote-splicing")
         && !IS_NULL (CDR (node)) && IS_NULL (CDDR (node));
}
