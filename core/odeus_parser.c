#include "odeus_parser.h"
#include "odeus_lexer.h"
#include "odeus_token.h"

/* expr = literal
 *      | '(' expr '.' expr ')'
 *      | list
 *      | quote
 */
static AST_Node *parse_expr (Lexer *lexer, Token *token);

/* list = '(' expr+ ')' */
static AST_Node *parse_list (Lexer *lexer);

/* literal = ATOM | INTEGER | FLOAT | STRING */
static AST_Node *parse_literal (Lexer *lexer, Token *token);

/* quote = '\'' expr  */
static AST_Node *parse_quote (Lexer *lexer, Token *token);

// helper functions for ast
static void ast_file_append (AST_Node *file, AST_Node *expr);
static AST_Node *make_cons (AST_Node *car, AST_Node *cdr);
static AST_Node *make_nil (void);

void
parser_panic (Token *token, const char *filename, const char *message)
{
  fprintf (stderr, "%s:%ld:%ld: %s", filename, token->line, token->column, message);
  exit (1);
}

Parser *
parser_init (Lexer *lexer)
{
  Parser *parser = malloc (sizeof (Parser));
  if (!parser)
    {
      fprintf (stderr, "ERROR: parser_init: Memory allocation error: %s", strerror (errno));
      exit (1);
    };

  parser->lexer = lexer;
  parser->file = malloc (sizeof (AST_Node));
  if (!parser->file)
    {
      fprintf (stderr, "ERROR: parser_init: Memory allocation error: %s", strerror (errno));
      free (parser);
      exit (1);
    };
  parser->file->type = AST_FILE;
  parser->file->value.FILE.COUNT = 0;
  parser->file->value.FILE.BODY = NULL;
  parser->file->line = 1;
  parser->file->column = 0;
  return parser;
}

void
parser_parse (Parser *parser)
{
  Token token = lexer_next_token (parser->lexer);
  while (token.type != TOKEN_END_OF_FILE)
    {
      AST_Node *expr = parse_expr (parser->lexer, &token);
      if (expr)
        ast_file_append (parser->file, expr);
      token = lexer_next_token (parser->lexer);
    }
}

static AST_Node *
parse_expr (Lexer *lexer, Token *token)
{
  switch (token->type)
    {
    case TOKEN_INTEGER: return parse_literal (lexer, token);
    case TOKEN_FLOAT: return parse_literal (lexer, token);
    case TOKEN_STRING: return parse_literal (lexer, token);
    case TOKEN_ATOM: return parse_literal (lexer, token);
    case TOKEN_OPEN_PAREN: return parse_list (lexer);
    case TOKEN_QUOTE: return parse_quote (lexer, token);

    case TOKEN_NONE:
    case TOKEN_CLOSE_PAREN:
    case TOKEN_PERIOD:
    case TOKEN_COMMA:
      {
        char message[256];
        snprintf (message, sizeof (message), "Unexpected token '%s'", token->value);
        parser_panic (token, lexer->filename, message);
        break;
      }

    case TOKEN_END_OF_FILE:
      {
        AST_Node *end_of_file = malloc (sizeof (AST_Node));
        if (!end_of_file)
          {
            fprintf (stderr, "ERROR: parser_parse: Memory allocation error: %s", strerror (errno));
            exit (1);
          };

        end_of_file->type = AST_END_OF_FILE;
        end_of_file->line = token->line;
        end_of_file->column = token->column;
        return end_of_file;
      }
    }
  return NULL;
}

static AST_Node *
parse_list (Lexer *lexer)
{ // Open parenthesis is already "consumed"
  AST_Node *head = NULL;
  AST_Node **tail = &head; // Pointer to the last CDR pointer

  Token token = lexer_next_token (lexer);
  while (token.type != TOKEN_END_OF_FILE && token.type != TOKEN_CLOSE_PAREN)
    {
      AST_Node *expr = parse_expr (lexer, &token);
      if (expr)
        {
          *tail = make_cons (expr, make_nil ());
          tail = &(*tail)->value.CONS.CDR; // Move tail to new CDR
        }
      token = lexer_next_token (lexer);
    }

  return head ? head : make_nil ();
}

static AST_Node *
make_nil (void)
{
  AST_Node *nil = malloc (sizeof (AST_Node));
  if (!nil)
    {
      fprintf (stderr, "ERROR: make_nil: Memory allocation error: %s", strerror (errno));
      exit (1);
    };
  nil->type = AST_NIL;
  nil->line = 1;
  nil->column = 0;
  return nil;
}

static AST_Node *
make_cons (AST_Node *car, AST_Node *cdr)
{
  AST_Node *cons = malloc (sizeof (AST_Node));
  if (!cons)
    {
      fprintf (stderr, "ERROR: make_cons: Memory allocation error: %s", strerror (errno));
      exit (1);
    };
  cons->type = AST_CONS;
  cons->value.CONS.CAR = car;
  cons->value.CONS.CDR = cdr;
  cons->line = car->line;
  cons->column = car->column;

  return cons;
}

static AST_Node *
parse_literal (Lexer *lexer, Token *token)
{
  AST_Node *literal = malloc (sizeof (AST_Node));
  if (!literal)
    {
      fprintf (stderr, "ERROR: parse_literal: Memory allocation error: %s", strerror (errno));
      exit (1);
    };
  literal->line = token->line;
  literal->column = token->column;

  switch (token->type)
    {
    case TOKEN_INTEGER:
      literal->type = AST_INTEGER;
      literal->value.INTEGER = strtol (token->value, NULL, 10);
      break;
    case TOKEN_FLOAT:
      literal->type = AST_FLOAT;
      literal->value.FLOAT = strtod (token->value, NULL);
      break;
    case TOKEN_STRING:
      literal->type = AST_STRING;
      literal->value.STRING = strdup (token->value);
      if (!literal->value.STRING)
        {
          fprintf (stderr, "ERROR: parse_literal: Memory allocation error: %s", strerror (errno));
          free (literal);
          exit (1);
        }
      break;
    case TOKEN_ATOM:
      literal->type = AST_ATOM;
      literal->value.ATOM = strdup (token->value);
      if (!literal->value.ATOM)
        {
          fprintf (stderr, "ERROR: parse_literal: Memory allocation error: %s", strerror (errno));
          free (literal);
          exit (1);
        }
      break;

    case TOKEN_NONE:
    case TOKEN_OPEN_PAREN:
    case TOKEN_CLOSE_PAREN:
    case TOKEN_QUOTE:
    case TOKEN_PERIOD:
    case TOKEN_COMMA:
    case TOKEN_END_OF_FILE:
      {
        char message[256];
        snprintf (message, sizeof (message), "Unexpected token '%s'", token->value);
        free (literal);
        parser_panic (token, lexer->filename, message);
      };
    }
  return literal;
}

static AST_Node *
parse_quote (Lexer *lexer, Token *token)
{
  AST_Node *quote = malloc (sizeof (AST_Node));
  if (!quote)
    {
      fprintf (stderr, "ERROR: parse_quote: Memory allocation error: %s", strerror (errno));
      exit (1);
    };
  quote->type = AST_QUOTE;
  quote->line = token->line;
  quote->column = token->column;

  Token next = lexer_next_token (lexer);

  AST_Node *expr = parse_expr (lexer, &next);
  if (!expr)
    {
      fprintf (stderr, "ERROR: parse_quote: Failed to parse quoted expression");
      free (quote);
      exit (1);
    }

  quote->value.QUOTE.EXPR = expr;
  return quote;
}

static void
ast_file_append (AST_Node *file, AST_Node *expr)
{
  file->value.FILE.BODY
      = realloc (file->value.FILE.BODY, sizeof (AST_Node *) * (file->value.FILE.COUNT + 1));
  if (!file->value.FILE.BODY)
    {
      fprintf (stderr, "ERROR: ast_file_append: Memory reallocation error: %s", strerror (errno));
      exit (1);
    }
  file->value.FILE.BODY[file->value.FILE.COUNT] = expr;
  file->value.FILE.COUNT++;
}

// printer

static void
print_indent (int level)
{
  for (int i = 0; i < level; i++)
    {
      printf ("  ");
    }
}

void
ast_print (AST_Node *node)
{
  if (node == NULL)
    {
      printf ("()");
      return;
    }

  switch (node->type)
    {
    case AST_NIL: printf ("()"); break;

    case AST_ATOM: printf ("%s", node->value.ATOM); break;

    case AST_INTEGER: printf ("%ld", node->value.INTEGER); break;

    case AST_FLOAT: printf ("%f", node->value.FLOAT); break;

    case AST_STRING: printf ("\"%s\"", node->value.STRING); break;

    case AST_QUOTE:
      printf ("'");
      ast_print (node->value.QUOTE.EXPR);
      break;

    case AST_CONS:
      printf ("(");
      ast_print (node->value.CONS.CAR);

      AST_Node *cdr = node->value.CONS.CDR;
      while (cdr != NULL && cdr->type == AST_CONS)
        {
          printf (" ");
          ast_print (cdr->value.CONS.CAR);
          cdr = cdr->value.CONS.CDR;
        }

      if (cdr != NULL && cdr->type != AST_NIL)
        {
          printf (" . ");
          ast_print (cdr);
        }

      printf (")");
      break;

    case AST_FILE: ast_print_file (node); break;

    case AST_END_OF_FILE: printf ("#<EOF>"); break;

    default: printf ("#<UNKNOWN>"); break;
    }
}

void
ast_print_pretty (AST_Node *node, int indent_level)
{
  if (node == NULL)
    {
      print_indent (indent_level);
      printf ("()\n");
      return;
    }

  switch (node->type)
    {
    case AST_NIL:
      print_indent (indent_level);
      printf ("()\n");
      break;

    case AST_ATOM:
      print_indent (indent_level);
      printf ("ATOM: %s\n", node->value.ATOM);
      break;

    case AST_INTEGER:
      print_indent (indent_level);
      printf ("INTEGER: %ld\n", node->value.INTEGER);
      break;

    case AST_FLOAT:
      print_indent (indent_level);
      printf ("FLOAT: %f\n", node->value.FLOAT);
      break;

    case AST_STRING:
      print_indent (indent_level);
      printf ("STRING: \"%s\"\n", node->value.STRING);
      break;

    case AST_QUOTE:
      print_indent (indent_level);
      printf ("QUOTE:\n");
      ast_print_pretty (node->value.QUOTE.EXPR, indent_level + 1);
      break;

    case AST_CONS:
      print_indent (indent_level);
      printf ("CONS:\n");
      print_indent (indent_level + 1);
      printf ("CAR:\n");
      ast_print_pretty (node->value.CONS.CAR, indent_level + 2);
      print_indent (indent_level + 1);
      printf ("CDR:\n");
      ast_print_pretty (node->value.CONS.CDR, indent_level + 2);
      break;

    case AST_FILE:
      print_indent (indent_level);
      printf ("FILE (%zu expressions):\n", node->value.FILE.COUNT);
      for (size_t i = 0; i < node->value.FILE.COUNT; i++)
        {
          ast_print_pretty (node->value.FILE.BODY[i], indent_level + 1);
        }
      break;

    case AST_END_OF_FILE:
      print_indent (indent_level);
      printf ("END_OF_FILE\n");
      break;

    default:
      print_indent (indent_level);
      printf ("UNKNOWN_NODE_TYPE\n");
      break;
    }
}

void
ast_print_file (AST_Node *file)
{
  if (file == NULL || file->type != AST_FILE)
    {
      printf ("Error: Expected AST_FILE node\n");
      return;
    }

  for (size_t i = 0; i < file->value.FILE.COUNT; i++)
    {
      ast_print (file->value.FILE.BODY[i]);
      printf ("\n");
    }
}
