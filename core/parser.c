#include "core/parser.h"

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

void
parser_panic (Parser *parser, Token *token, const char *message)
{
  parser->error.filename = parser->lexer->filename;
  parser->error.status = ERROR;
  parser->error.message = message;
  parser->error.line = token->line;
  parser->error.column = token->column;
}

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
  parser->start_node = nil ();

  return parser;
}

AST *
parser_parse (Parser *parser)
{
  AST *begin_head = nil ();
  AST **begin_tail = &begin_head;

  Token token = lexer_next_token (parser->lexer);

  while (token.type != TOKEN_END_OF_FILE)
    {
      AST *expr = parse_expr (parser, &token);
      if (expr)
        {
          *begin_tail = make_cons (expr, nil ());
          begin_tail = &CDR (*begin_tail);
        }
    }

  return make_cons (make_symbol ("begin"), begin_head);
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
        snprintf (message, sizeof (message), "Unexpected token '%s'",
                  token->value);
        parser_panic (parser, token, message);
      }
      break;

    case TOKEN_END_OF_FILE:
      {
        AST *eof = malloc (sizeof (AST));
        eof->type = AST_END_OF_FILE;
        return eof;
      }
    }
  return NULL;
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
            parser_panic (parser, token, "Dot without CAR");

          *tail = cdr;

          if (token->type != TOKEN_CLOSE_PAREN)
            parser_panic (parser, token, "Expected ')' after dotted pair\n");

          *token = lexer_next_token (parser->lexer);
          return head;
        }

      AST *expr = parse_expr (parser, token);
      *tail = make_cons (expr, nil ());
      tail = &CDR (*tail);
    }

  if (token->type != TOKEN_CLOSE_PAREN)
    parser_panic (parser, token, "Unterminated list");

  *token = lexer_next_token (parser->lexer); // consume ')'
  return head ? head : nil ();
}

static AST *
parse_literal (Parser *parser, Token *token)
{
  AST *n = NULL;

  switch (token->type)
    {
    case TOKEN_INTEGER:
      n = make_integer (strtol (token->value, NULL, 10));
      break;
    case TOKEN_FLOAT:
      n = make_float (strtod (token->value, NULL));
      break;
    case TOKEN_STRING:
      n = make_string (token->value);
      break;
    case TOKEN_SYMBOL:
      n = make_symbol (token->value);
      break;

    default:
      parser_panic (parser, token, "Unexpected literal");
      return nil ();
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

  return make_cons (make_symbol ("quote"), make_cons (expr, nil ()));
}

static AST *
parse_quasiquote (Parser *parser, Token *token)
{
  // consume `
  *token = lexer_next_token (parser->lexer);

  AST *expr = parse_expr (parser, token);

  return make_cons (make_symbol ("quasiquote"), make_cons (expr, nil ()));
}

static AST *
parse_unquote (Parser *parser, Token *token)
{
  // consume ,
  *token = lexer_next_token (parser->lexer);

  AST *expr = parse_expr (parser, token);

  return make_cons (make_symbol ("unquote"), make_cons (expr, nil ()));
}

static AST *
parse_unquote_splicing (Parser *parser, Token *token)
{
  // consume ,@
  *token = lexer_next_token (parser->lexer);

  AST *expr = parse_expr (parser, token);

  return make_cons (make_symbol ("unquote-splicing"),
                    make_cons (expr, nil ()));
}
