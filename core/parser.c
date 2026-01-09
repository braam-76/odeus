#include "odeus_parser.h"

/* expr = literal
 *      | '(' expr '.' expr ')'
 *      | list
 *      | quote
 */
static AST *parse_expr (Parser *parser, Token *token);

/* list = '(' expr+ ')' */
static AST *parse_list (Parser *parser, Token *token);

/* literal = SYMBOL | INTEGER | FLOAT | STRING */
static AST *parse_literal (Parser *parser, Token *token);

/* quote = '\'' expr  */
static AST *parse_quote (Parser *parser, Token *token);

void
parser_panic (Parser *parser, Token *token, const char *message)
{
  parser->error.filename = parser->lexer->filename;
  parser->error.status = ODEUS_ERROR;
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

  AST *global_environment = make_cons (nil (), nil ());
  AST *program = nil ();
  parser->start_node = make_cons (global_environment, program);

  return parser;
}

void
parser_free (Parser *parser)
{
  if (!parser)
    return;
  ast_free (parser->start_node);
  lexer_free (parser->lexer);
  free (parser);
}

/* ----------------------------------------------------------
 *   TOP-LEVEL PARSING
 * ---------------------------------------------------------- */
void
parser_parse (Parser *parser)
{
  AST *begin_head = nil ();
  AST **begin_tail = &begin_head;

  // initial token
  Token token = lexer_next_token (parser->lexer);

  while (token.type != TOKEN_END_OF_FILE)
    {
      AST *expr = parse_expr (parser, &token); // token is updated internally

      if (expr)
        {
          *begin_tail = make_cons (expr, nil ());
          begin_tail = &CDR (*begin_tail);
        }
    }
  AST *begin_symbol = (AST *)malloc (sizeof (AST));
  begin_symbol->type = AST_SYMBOL;
  begin_symbol->as.SYMBOL = "begin";
  AST *begin_node = make_cons (begin_symbol, begin_head);

  CDR (parser->start_node) = begin_node;
}

/* ----------------------------------------------------------
 *   PARSER HELPERS
 * ---------------------------------------------------------- */

static AST *
parse_expr (Parser *parser, Token *token)
{
  switch (token->type)
    {
    case TOKEN_INTEGER:
    case TOKEN_FLOAT:
    case TOKEN_STRING:
    case TOKEN_SYMBOL: return parse_literal (parser, token);

    case TOKEN_OPEN_PAREN: return parse_list (parser, token);

    case TOKEN_QUOTE: return parse_quote (parser, token);

    case TOKEN_NONE:
    case TOKEN_CLOSE_PAREN:
    case TOKEN_PERIOD:
    case TOKEN_COMMA:
      {
        char message[256];
        snprintf (message, sizeof (message), "Unexpected token '%s'", token->value);
        parser_panic (parser, token, message);
      }
      break;

    case TOKEN_END_OF_FILE:
      {
        AST *eof = malloc (sizeof (AST));
        eof->type = AST_END_OF_FILE;
        eof->line = token->line;
        eof->column = token->column;
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

AST *
make_cons (AST *car, AST *cdr)
{
  AST *n = malloc (sizeof (AST));
  n->type = AST_CONS;
  CAR (n) = car;
  CDR (n) = cdr;
  n->line = car->line;
  n->column = car->column;
  return n;
}

static AST *
parse_literal (Parser *parser, Token *token)
{
  AST *n = malloc (sizeof (AST));
  n->line = token->line;
  n->column = token->column;

  switch (token->type)
    {
    case TOKEN_INTEGER:
      n->type = AST_INTEGER;
      n->as.INTEGER = strtol (token->value, NULL, 10);
      break;

    case TOKEN_FLOAT:
      n->type = AST_FLOAT;
      n->as.FLOAT = strtod (token->value, NULL);
      break;

    case TOKEN_STRING:
      n->type = AST_STRING;
      n->as.STRING = strdup (token->value);
      break;

    case TOKEN_SYMBOL:
      n->type = AST_SYMBOL;
      n->as.SYMBOL = strdup (token->value);
      break;

    default:
      {
        char msg[256];
        snprintf (msg, sizeof (msg), "Unexpected literal token '%s'", token->value);
        free (n);
        parser_panic (parser, token, msg);
      }
    }

  *token = lexer_next_token (parser->lexer);

  return n;
}

static AST *
parse_quote (Parser *parser, Token *token)
{
  AST *n = malloc (sizeof (AST));
  n->type = AST_QUOTE;
  n->line = token->line;
  n->column = token->column;

  *token = lexer_next_token (parser->lexer);

  n->as.QUOTE.EXPR = parse_expr (parser, token);

  return n;
}
