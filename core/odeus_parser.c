/*
 *
 *
 */

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
static AST_Node *parse_list (Lexer *lexer, Token *token);

/* literal = SYMBOL | INTEGER | FLOAT | STRING */
static AST_Node *parse_literal (Lexer *lexer, Token *token);

/* quote = '\'' expr  */
static AST_Node *parse_quote (Lexer *lexer, Token *token);

static AST_Node *GLOBAL_NIL = NULL;

AST_Node *
nil (void)
{
  if (!GLOBAL_NIL)
    {
      GLOBAL_NIL = malloc (sizeof (AST_Node));
      GLOBAL_NIL->type = AST_NIL;
      GLOBAL_NIL->line = 0;
      GLOBAL_NIL->column = 0;
    }
  return GLOBAL_NIL;
}

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
      fprintf (stderr, "ERROR: parser_init: %s", strerror (errno));
      exit (1);
    }

  parser->lexer = lexer;

  AST_Node *global_environment = cons (nil (), nil ());
  AST_Node *program = nil ();
  parser->start_node = cons (global_environment, program);

  return parser;
}

void
ast_free (AST_Node *node)
{
  if (!node)
    return;

  switch (node->type)
    {
    case AST_STRING: free (node->as.STRING); break;

    case AST_SYMBOL: free (node->as.SYMBOL); break;

    case AST_CONS:
      ast_free (CAR (node));
      ast_free (CDR (node));
      break;

    case AST_QUOTE: ast_free (node->as.QUOTE.EXPR); break;

    default: break;
    }

  free (node);
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
  AST_Node *begin_head = nil ();
  AST_Node **begin_tail = &begin_head;

  // initial token
  Token token = lexer_next_token (parser->lexer);

  while (token.type != TOKEN_END_OF_FILE)
    {
      AST_Node *expr = parse_expr (parser->lexer, &token); // token is updated internally

      if (expr)
        {
          *begin_tail = cons (expr, nil ());
          begin_tail = &CDR (*begin_tail);
        }
    }
  AST_Node *begin_symbol = (AST_Node *)malloc (sizeof (AST_Node));
  begin_symbol->type = AST_SYMBOL;
  begin_symbol->as.SYMBOL = "BEGIN";
  AST_Node *begin_node = cons (begin_symbol, begin_head);

  CDR(parser->start_node) = begin_node;
}

/* ----------------------------------------------------------
 *   PARSER HELPERS
 * ---------------------------------------------------------- */

static AST_Node *
parse_expr (Lexer *lexer, Token *token)
{
  switch (token->type)
    {
    case TOKEN_INTEGER:
    case TOKEN_FLOAT:
    case TOKEN_STRING:
    case TOKEN_SYMBOL: return parse_literal (lexer, token);

    case TOKEN_OPEN_PAREN: return parse_list (lexer, token);

    case TOKEN_QUOTE: return parse_quote (lexer, token);

    case TOKEN_NONE:
    case TOKEN_CLOSE_PAREN:
    case TOKEN_PERIOD:
    case TOKEN_COMMA:
      {
        char msg[256];
        snprintf (msg, sizeof (msg), "Unexpected token '%s'", token->value);
        parser_panic (token, lexer->filename, msg);
      }
      break;

    case TOKEN_END_OF_FILE:
      {
        AST_Node *eof = malloc (sizeof (AST_Node));
        eof->type = AST_END_OF_FILE;
        eof->line = token->line;
        eof->column = token->column;
        return eof;
      }
    }
  return NULL;
}

static AST_Node *
parse_list (Lexer *lexer, Token *token)
{
  // consume '('
  *token = lexer_next_token (lexer);

  AST_Node *head = NULL;
  AST_Node **tail = &head;

  while (token->type != TOKEN_END_OF_FILE && token->type != TOKEN_CLOSE_PAREN)
    {
      if (token->type == TOKEN_PERIOD)
        {
          // dot notation: CAR already exists, parse CDR
          *token = lexer_next_token (lexer); // consume '.'
          AST_Node *cdr = parse_expr (lexer, token);

          if (!head)
            parser_panic (token, lexer->filename, "Dot without CAR");

          *tail = cdr;

          if (token->type != TOKEN_CLOSE_PAREN)
            *token = lexer_next_token (lexer);

          return head;
        }

      AST_Node *expr = parse_expr (lexer, token);
      *tail = cons (expr, nil ());
      tail = &CDR (*tail);
    }

  if (token->type != TOKEN_CLOSE_PAREN)
    parser_panic (token, lexer->filename, "Unterminated list");

  *token = lexer_next_token (lexer); // consume ')'
  return head ? head : nil ();
}

AST_Node *
cons (AST_Node *car, AST_Node *cdr)
{
  AST_Node *n = malloc (sizeof (AST_Node));
  n->type = AST_CONS;
  CAR (n) = car;
  CDR (n) = cdr;
  n->line = car->line;
  n->column = car->column;
  return n;
}

static AST_Node *
parse_literal (Lexer *lexer, Token *token)
{
  AST_Node *n = malloc (sizeof (AST_Node));
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
        parser_panic (token, lexer->filename, msg);
      }
    }

  *token = lexer_next_token (lexer);

  return n;
}

static AST_Node *
parse_quote (Lexer *lexer, Token *token)
{
  AST_Node *n = malloc (sizeof (AST_Node));
  n->type = AST_QUOTE;
  n->line = token->line;
  n->column = token->column;

  *token = lexer_next_token (lexer);

  n->as.QUOTE.EXPR = parse_expr (lexer, token);

  return n;
}

/* ----------------------------------------------------------
 *   PRINTING
 * ---------------------------------------------------------- */

void
ast_print (AST_Node *node)
{
  if (!node)
    {
      printf ("()");
      return;
    }

  switch (node->type)
    {
    case AST_NIL: printf ("nil"); break;

    case AST_SYMBOL: printf ("%s", node->as.SYMBOL); break;

    case AST_INTEGER: printf ("%ld", node->as.INTEGER); break;

    case AST_FLOAT: printf ("%f", node->as.FLOAT); break;

    case AST_STRING: printf ("\"%s\"", node->as.STRING); break;

    case AST_QUOTE:
      printf ("'");
      ast_print (node->as.QUOTE.EXPR);
      break;

    case AST_CONS:
      {
        printf ("(");
        AST_Node *cur = node;

        while (cur->type == AST_CONS)
          {
            ast_print (CAR (cur));
            cur = CDR (cur);

            if (cur->type == AST_CONS)
              printf (" ");
          }

        if (cur->type != AST_NIL)
          {

            printf (" . ");
            ast_print (cur);
          }
            printf (")");
        break;
      }

    case AST_END_OF_FILE: printf ("#<EOF>"); break;

    default: printf ("#<UNKNOWN>"); break;
    }
}
