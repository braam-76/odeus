#include "odeus_eval.h"

void
environment_set (AST_Node *env, AST_Node *symbol, AST_Node *value)
{
  AST_Node *variables = CDR (env);

  while (variables->type != AST_NIL)
    {
      AST_Node *pair = CAR (variables); // (symbol . value)
      AST_Node *key = CAR (pair);

      if (strcmp (key->as.SYMBOL, symbol->as.SYMBOL) == 0)
        {
          CDR (pair) = value; // update existing binding
          return;
        }

      variables = CDR (variables);
    }

  // Prepend (symbol . value)
  AST_Node *pair = cons (symbol, value);
  CDR (env) = cons (pair, CDR (env));
}

AST_Node *
environment_get (AST_Node *env, AST_Node *symbol)
{
  AST_Node *variables = CDR (env);
  AST_Node *parent = CAR (env);

  while (variables->type != AST_NIL)
    {
      AST_Node *pair = CAR (variables);
      AST_Node *key = CAR (pair);

      if (strcmp (key->as.SYMBOL, symbol->as.SYMBOL) == 0)
        return CDR (pair);

      variables = CDR (variables);
    }

  if (parent->type == AST_NIL)
    {
      fprintf (stderr, "ERROR: Unbound symbol: %s\n", symbol->as.SYMBOL);
      exit (1);
    }

  return environment_get (parent, symbol);
}

/* Small helpers used by evaluator */

static AST_Node *
make_integer (long value)
{
  AST_Node *node = malloc (sizeof (AST_Node));
  node->type = AST_INTEGER;
  node->as.INTEGER = value;
  node->line = 0;
  node->column = 0;
  return node;
}

static AST_Node *
make_float (double value)
{
  AST_Node *node = malloc (sizeof (AST_Node));
  node->type = AST_FLOAT;
  node->as.FLOAT = value;
  node->line = 0;
  node->column = 0;
  return node;
}

static AST_Node *
make_string (const char *string)
{
  AST_Node *node = malloc (sizeof (AST_Node));
  node->type = AST_STRING;
  node->as.STRING = strdup (string);
  node->line = 0;
  node->column = 0;
  return node;
}

static AST_Node *
make_symbol (const char *symbol)
{
  AST_Node *node = malloc (sizeof (AST_Node));
  node->type = AST_SYMBOL;
  node->as.SYMBOL = strdup (symbol);
  node->line = 0;
  node->column = 0;
  return node;
}

/* Forward declaration */
AST_Node *evaluate_expression (AST_Node *environment, AST_Node *expression);

static AST_Node *
evaluate_arguments (AST_Node *environment, AST_Node *arguments)
{
  AST_Node *head = nil ();
  AST_Node *tail = NULL;

  while (arguments->type == AST_CONS)
    {
      AST_Node *evaluated = evaluate_expression (environment, CAR (arguments));
      AST_Node *node = cons (evaluated, nil ());
      if (tail)
        CDR (tail) = node;
      else
        head = node;
      tail = node;
      arguments = CDR (arguments);
    }

  return head ? head : nil ();
}

AST_Node *
evaluate_program (Parser *parser)
{
  if (!parser || !parser->start_node)
    return nil ();

  AST_Node *global_environment = CAR (parser->start_node);
  AST_Node *program = CDR (parser->start_node);
  AST_Node *last = nil ();

  while (program->type == AST_CONS)
    {
      AST_Node *expression = CAR (program);
      last = evaluate_expression (global_environment, expression);
      program = CDR (program);
    }

  return last;
}

/* Main evaluator */
AST_Node *
evaluate_expression (AST_Node *environment, AST_Node *expression)
{
  if (!expression)
    return nil ();

  switch (expression->type)
    {
    case AST_INTEGER:
    case AST_FLOAT:
    case AST_STRING:
    case AST_QUOTE: return expression;

    case AST_SYMBOL: return environment_get (environment, expression);

    case AST_NIL: return expression;

    case AST_CONS:
      {
        AST_Node *op = CAR (expression);
        AST_Node *args = CDR (expression);

        /* --- If operator is a symbol, check special forms BEFORE evaluating op --- */
        if (op->type == AST_SYMBOL)
          {
            if (strcmp (op->as.SYMBOL, "BEGIN") == 0)
              {

                AST_Node *result = nil ();
                AST_Node *a = args;
                while (a->type == AST_CONS)
                  {
                    result = evaluate_expression (environment, CAR (a));
                    a = CDR (a);
                  }
                return result;
              }

            if (strcmp (op->as.SYMBOL, "QUOTE") == 0)
              {
                if (IS_NULL (args) || !IS_NULL (CDR (args)))
                  {
                    fprintf (stderr, "ERROR: QUOTE expects 1 argument\n");
                    exit (1);
                  }
                return CAR (args);
              }

            if (strcmp (op->as.SYMBOL, "DEFINE") == 0)
              {
                /* (define sym expr) */
                if (IS_NULL (args) || IS_NULL (CDR (args)))
                  {
                    fprintf (stderr, "ERROR: DEFINE expects (symbol value)\n");
                    exit (1);
                  }

                AST_Node *sym = CAR (args);
                if (sym->type != AST_SYMBOL)
                  {
                    fprintf (stderr, "ERROR: first argument for DEFINE must be a symbol\n");
                    exit (1);
                  }

                AST_Node *value_expr = CAR (CDR (args));
                AST_Node *value = evaluate_expression (environment, value_expr);
                environment_set (environment, sym, value);
                return sym;
              }

            if (strcmp (op->as.SYMBOL, "PRINT") == 0)
              {
                AST_Node *a = args;
                while (a->type == AST_CONS)
                  {
                    AST_Node *v = evaluate_expression (environment, CAR (a));
                    ast_print (v);
                    printf (" ");
                    a = CDR (a);
                  }
                printf ("\n");
                return nil ();
              }
            if (strcmp (op->as.SYMBOL, "+") == 0)
              {
                if (IS_NULL (args))
                  {
                    fprintf (stderr, "ERROR: + expects at least one argument\n");
                    exit (1);
                  }

                double acc = 0.0;
                int any_float = 0;
                AST_Node *a = args;
                while (a->type == AST_CONS)
                  {
                    AST_Node *v = evaluate_expression (environment, CAR (a));
                    if (v->type == AST_INTEGER)
                      acc += (double)v->as.INTEGER;
                    else if (v->type == AST_FLOAT)
                      {
                        acc += v->as.FLOAT;
                        any_float = 1;
                      }
                    else
                      {
                        fprintf (stderr, "ERROR: + expects numeric arguments\n");
                        exit (1);
                      }
                    a = CDR (a);
                  }
                if (any_float)
                  return make_float (acc);
                else
                  return make_integer ((long)acc);
              }

            if (strcmp (op->as.SYMBOL, "-") == 0)
              {
                if (IS_NULL (args))
                  {
                    fprintf (stderr, "ERROR: - expects at least one argument\n");
                    exit (1);
                  }

                double acc = 0.0;
                int any_float = 0;
                AST_Node *a = args;

                AST_Node *first = evaluate_expression (environment, CAR (a));
                if (first->type == AST_INTEGER)
                  acc = (double)first->as.INTEGER;
                else if (first->type == AST_FLOAT)
                  {
                    acc = first->as.FLOAT;
                    any_float = 1;
                  }
                else
                  {
                    fprintf (stderr, "ERROR: - expects numeric arguments\n");
                    exit (1);
                  }
                a = CDR (a);

                while (a->type == AST_CONS)
                  {
                    AST_Node *v = evaluate_expression (environment, CAR (a));
                    if (v->type == AST_INTEGER)
                      acc -= (double)v->as.INTEGER;
                    else if (v->type == AST_FLOAT)
                      {
                        acc -= v->as.FLOAT;
                        any_float = 1;
                      }
                    else
                      {
                        fprintf (stderr, "ERROR: - expects numeric arguments\n");
                        exit (1);
                      }
                    a = CDR (a);
                  }

                if (any_float)
                  return make_float (acc);
                else
                  return make_integer ((long)acc);
              }

            if (strcmp (op->as.SYMBOL, "*") == 0)
              {
                if (IS_NULL (args))
                  {
                    fprintf (stderr, "ERROR: * expects at least one argument\n");
                    exit (1);
                  }

                double acc = 1.0;
                int any_float = 0;
                AST_Node *a = args;
                while (a->type == AST_CONS)
                  {
                    AST_Node *v = evaluate_expression (environment, CAR (a));
                    if (v->type == AST_INTEGER)
                      acc *= (double)v->as.INTEGER;
                    else if (v->type == AST_FLOAT)
                      {
                        acc *= v->as.FLOAT;
                        any_float = 1;
                      }
                    else
                      {
                        fprintf (stderr, "ERROR: * expects numeric arguments\n");
                        exit (1);
                      }
                    a = CDR (a);
                  }
                if (any_float)
                  return make_float (acc);
                else
                  return make_integer ((long)acc);
              }

            if (strcmp (op->as.SYMBOL, "/") == 0)
              {
                if (IS_NULL (args))
                  {
                    fprintf (stderr, "ERROR: / expects at least one argument\n");
                    exit (1);
                  }

                AST_Node *a = args;
                AST_Node *first = evaluate_expression (environment, CAR (a));
                double acc;

                if (first->type == AST_INTEGER)
                  acc = (double)first->as.INTEGER;
                else if (first->type == AST_FLOAT)
                  acc = first->as.FLOAT;
                else
                  {
                    fprintf (stderr, "ERROR: / expects numeric arguments\n");
                    exit (1);
                  }

                a = CDR (a);

                while (a->type == AST_CONS)
                  {
                    AST_Node *v = evaluate_expression (environment, CAR (a));
                    double val;

                    if (v->type == AST_INTEGER)
                      val = (double)v->as.INTEGER;
                    else if (v->type == AST_FLOAT)

                      val = v->as.FLOAT;

                    else
                      {
                        fprintf (stderr, "ERROR: / expects numeric arguments\n");
                        exit (1);
                      }

                    if (val == 0.0)
                      {
                        fprintf (stderr, "ERROR: division by zero\n");
                        exit (1);
                      }

                    acc /= val;
                    a = CDR (a);
                  }

                return make_float (acc);
              }
          }

        /* --- For non-special-form calls: evaluate operator then apply --- */

        AST_Node *op_val = evaluate_expression (environment, op);

        /* If after evaluation operator is a SYMBOL bound to something, try to get it */
        if (op_val && op_val->type == AST_SYMBOL)
          {
            op_val = environment_get (environment, op_val);
          }

        // TODO: add lambdas (important to make language actially usefull)

        /* If operator is not a function we know about, error */
        fprintf (stderr, "ERROR: Unknown function/operator: ");
        if (op->type == AST_SYMBOL)
          fprintf (stderr, "'%s'\n", op->as.SYMBOL);
        else
          {
            fprintf (stderr, "(non-symbol operator)\n");
            ast_print (expression);
            printf ("\n");
            fprintf (stderr, "\n");
          }
        exit (1);
      }

    case AST_END_OF_FILE: return expression;

    default:
      fprintf (stderr, "ERROR: evaluate_expression: unhandled AST type %d\n", expression->type);
      exit (1);
    }
}
