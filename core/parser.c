#include "core/parser.h"
#include "core/ast.h"

/* expr = literal
 *      | '(' expr '.' expr ')'
 *      | list
 *      | quote
 *      | quasiquote
 */
static AST *parse_expr (Parser *parser, Token *token);

/* list = '(' expr+ ')' */
static AST *parse_list (Parser *parser, Token *token);

/* literal = SYMBOL | INTEGER | FLOAT | STRING */
static AST *parse_literal (Parser *parser, Token *token);

/* quote = '\'' expr  */
static AST *parse_quote (Parser *parser, Token *token);

static AST *parse_quasiquote (Parser *parser, Token *token);
static AST *parse_unquote (Parser *parser, Token *token);
static AST *parse_unquote_splicing (Parser *parser, Token *token);

Parser *
parser_init (Lexer *lexer)
{
  Parser *parser = malloc (sizeof (Parser));
  if (!parser)
    {
      fprintf (stderr, "ERROR: parser_init: %s", strerror (errno));
      exit (1);
    }

  parser->lexer = lexer;
  parser->start_node = ast_nil ();

  return parser;
}

AST *
parser_parse (Parser *parser)
{
  AST *begin_head = ast_nil ();
  AST **begin_tail = &begin_head;

  Token token = lexer_next_token (parser->lexer);

  while (token.type != TOKEN_END_OF_FILE)
    {
      AST *expr = parse_expr (parser, &token);
      if (!expr || expr->type == AST_ERROR)
        return expr;

      if (expr)
        {
          *begin_tail = ast_cons (expr, ast_nil ());
          begin_tail = &CDR (*begin_tail);
        }
    }

  return ast_cons (ast_symbol ("begin"), begin_head);
}

static AST *
parse_expr (Parser *parser, Token *token)
{
  switch (token->type)
    {
    case TOKEN_INTEGER:
    case TOKEN_FLOAT:
    case TOKEN_STRING:
    case TOKEN_SYMBOL:
      return parse_literal (parser, token);

    case TOKEN_OPEN_PAREN:
      return parse_list (parser, token);

    case TOKEN_QUOTE:
      return parse_quote (parser, token);

    case TOKEN_QUASIQUOTE:
      return parse_quasiquote (parser, token);

    case TOKEN_UNQUOTE:
      return parse_unquote (parser, token);

    case TOKEN_UNQUOTE_SPLICING:
      return parse_unquote_splicing (parser, token);

    case TOKEN_NONE:
    case TOKEN_CLOSE_PAREN:
    case TOKEN_PERIOD:
      {
        char message[256];
        snprintf (message, sizeof (message),
                  "ERROR: parse_expr: Unexpected token '%s'", token->value);
        return ast_error (message);
      }
      break;

    default:
      return ast_error ("ERROR: parse_expr: Unreachable");
    }
}

static AST *
parse_list (Parser *parser, Token *token)
{
  // consume '('
  *token = lexer_next_token (parser->lexer);

  AST *head = NULL;
  AST **tail = &head;

  while (token->type != TOKEN_END_OF_FILE && token->type != TOKEN_CLOSE_PAREN)
    {
      if (token->type == TOKEN_PERIOD)
        {
          // dot notation: CAR already exists, parse CDR
          *token = lexer_next_token (parser->lexer); // consume '.'
          AST *cdr = parse_expr (parser, token);

          if (!head)
            return ast_error ("Dot without CAR");

          *tail = cdr;

          if (token->type != TOKEN_CLOSE_PAREN)
            return ast_error ("Expected ')' after dotted pair\n");

          *token = lexer_next_token (parser->lexer);
          return head;
        }

      AST *expr = parse_expr (parser, token);
      *tail = ast_cons (expr, ast_nil ());
      tail = &CDR (*tail);
    }

  if (token->type != TOKEN_CLOSE_PAREN)
    return ast_error ("Unterminated list");

  *token = lexer_next_token (parser->lexer); // consume ')'
  return head ? head : ast_nil ();
}

static AST *
parse_literal (Parser *parser, Token *token)
{
  AST *n = NULL;

  switch (token->type)
    {
    case TOKEN_INTEGER:
      n = ast_integer (strtol (token->value, NULL, 10));
      break;
    case TOKEN_FLOAT:
      n = ast_float (strtod (token->value, NULL));
      break;
    case TOKEN_STRING:
      n = ast_string (token->value);
      break;
    case TOKEN_SYMBOL:
      n = ast_symbol (token->value);
      break;

    default:
      ast_error ("Unexpected literal");
      return ast_nil ();
    }

  *token = lexer_next_token (parser->lexer);
  return n;
}

static AST *
parse_quote (Parser *parser, Token *token)
{
  // consume '
  *token = lexer_next_token (parser->lexer);

  AST *expr = parse_expr (parser, token);

  return ast_cons (ast_symbol ("quote"), ast_cons (expr, ast_nil ()));
}

static AST *
parse_quasiquote (Parser *parser, Token *token)
{
  // consume `
  *token = lexer_next_token (parser->lexer);

  AST *expr = parse_expr (parser, token);

  return ast_cons (ast_symbol ("quasiquote"), ast_cons (expr, ast_nil ()));
}

static AST *
parse_unquote (Parser *parser, Token *token)
{
  // consume ,
  *token = lexer_next_token (parser->lexer);

  AST *expr = parse_expr (parser, token);

  return ast_cons (ast_symbol ("unquote"), ast_cons (expr, ast_nil ()));
}

static AST *
parse_unquote_splicing (Parser *parser, Token *token)
{
  // consume ,@
  *token = lexer_next_token (parser->lexer);

  AST *expr = parse_expr (parser, token);

  return ast_cons (ast_symbol ("unquote-splicing"),
                   ast_cons (expr, ast_nil ()));
}
