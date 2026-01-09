#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ───────────────────────────────
// Internal helpers
// ───────────────────────────────

static char peek (Lexer *lexer);
static char peek_next (Lexer *lexer, int n);
static void advance (Lexer *lexer);
static void panic (Lexer *lexer, const char *message);
static bool is_allowed_for_symbol (char ch);
static char *copy_substring (const char *src, size_t start, size_t end);

static Token string_token (Lexer *lexer);
static Token number_token (Lexer *lexer);
static Token symbol_token (Lexer *lexer);

static Token create_token (Lexer *lexer, Token_Type type);
static Token create_token_with_value (Lexer *lexer, Token_Type type, char *value);

// ───────────────────────────────
// Lexer
// ───────────────────────────────

Lexer
lexer_from_file (char *filename, char *source, size_t source_size)
{
  Lexer lexer = { 0 };

  lexer.filename = filename;
  lexer.source = source;
  lexer.source_size = source_size;
  lexer.position = 0;
  lexer.line = 1;
  lexer.column = 0;
  lexer.token_start_column = 0;

  return lexer;
}

Lexer
lexer_from_string (char *source, size_t source_size)
{
  Lexer lexer = { 0 };

  lexer.filename = "#<from_string>";
  lexer.source = source;
  lexer.source_size = source_size;
  lexer.position = 0;
  lexer.line = 1;
  lexer.column = 0;
  lexer.token_start_column = 0;

  return lexer;
}


void
lexer_free (Lexer *lexer)
{
  free (lexer->filename);
  free (lexer->source);
  free (lexer);
}

// ───────────────────────────────
// Tokenization core
// ───────────────────────────────

Token
lexer_next_token (Lexer *lexer)
{
  Token token_to_return = { 0 };
  while (true)
    { // record start column before consuming
      lexer->token_start_column = lexer->column - 1;
      switch (peek (lexer))
        {
        // Whitespace
        case ' ':
        case '\t':
        case '\r': advance (lexer); break;

        case '\n':
          lexer->line++;
          lexer->column = 0;
          advance (lexer);
          break;

        // Single-character tokens
        case '(': token_to_return = create_token (lexer, TOKEN_OPEN_PAREN); break;
        case ')': token_to_return = create_token (lexer, TOKEN_CLOSE_PAREN); break;
        case ',': token_to_return = create_token (lexer, TOKEN_COMMA); break;
        case '\'': token_to_return = create_token (lexer, TOKEN_QUOTE); break;

        case '.':
          if (is_allowed_for_symbol (peek_next (lexer, 1)))
            return symbol_token (lexer);
          token_to_return = create_token (lexer, TOKEN_PERIOD);
          break;

        // Comments
        case ';':
          while (peek (lexer) != '\n')
            advance (lexer);
          continue;

        // Strings
        case '"': return string_token (lexer);

        // End of file
        case '\0': token_to_return = create_token (lexer, TOKEN_END_OF_FILE); break;

        // Numbers or symbols
        default:
          if (isdigit (peek (lexer)) || (peek (lexer) == '-' && isdigit (peek_next (lexer, 1))))
            return number_token (lexer);
          else if (is_allowed_for_symbol (peek (lexer)))
            return symbol_token (lexer);
          else
            {
              char buf[64];
              snprintf (buf, sizeof (buf), "Unknown character: '%c'", peek (lexer));
              panic (lexer, buf);
              return (Token){.type = TOKEN_NONE};
            }
        }

      if (token_to_return.type != TOKEN_NONE)
        break;
    }

  advance (lexer);
  return token_to_return;
}

// ───────────────────────────────
// Helper functions
// ───────────────────────────────

static inline char
peek (Lexer *lexer)
{
  return peek_next (lexer, 0);
}

static char
peek_next (Lexer *lexer, int n)
{
  size_t pos = lexer->position + n;
  if (pos >= lexer->source_size)
    return '\0';
  return lexer->source[pos];
}

static void
advance (Lexer *lexer)
{
  lexer->position++;
  lexer->column++;
}

static void
panic (Lexer *lexer, const char *message)
{
  lexer->error.status = ERROR;
  lexer->error.message = message;
  lexer->error.filename = lexer->filename;
  lexer->error.line = lexer->line;
  lexer->error.column = lexer->column;
}

static char *
copy_substring (const char *src, size_t start, size_t end)
{
  size_t length = end - start;
  char *s = malloc (length + 1);
  if (!s)
    {
      fprintf (stderr, "Out of memory\n");
      exit (EXIT_FAILURE);
    }
  memcpy (s, src + start, length);
  s[length] = '\0';
  return s;
}

static Token
create_token (Lexer *lexer, Token_Type type)
{
  return create_token_with_value (lexer, type, NULL);
}

static Token
create_token_with_value (Lexer *lexer, Token_Type type, char *value)
{
  Token token = { 0 };
  token.type = type;
  token.value = value;
  token.line = lexer->line;

  token.column = lexer->token_start_column;

  return token;
}

// ───────────────────────────────
// Token parsing functions
// ───────────────────────────────

static Token
string_token (Lexer *lexer)
{
  advance (lexer); // skip opening quote

  size_t capacity = 64;
  size_t length = 0;
  char *buffer = malloc (capacity);
  if (!buffer)
    panic (lexer, "out of memory");

  while (peek (lexer) != '"' && peek (lexer) != '\0')
    {
      if (peek (lexer) == '\n')
        panic (lexer, "incomplete string");

      char c = ' ';

      if (peek (lexer) == '\\')
        {
          advance (lexer);
          switch (peek (lexer))
            {
            case 'n': c = '\n'; break;
            case 't': c = '\t'; break;
            case 'r': c = '\r'; break;
            case '"': c = '"'; break;
            case '\'': c = '\''; break;
            case '\\': c = '\\'; break;
            default: panic (lexer, "illegal character to escape");
            }
        }
      else
        c = peek (lexer);

      if (length + 1 >= capacity)
        {
          capacity *= 2;
          char *newbuf = realloc (buffer, capacity);
          if (!newbuf)
            {
              free (buffer);
              panic (lexer, "out of memory while parsing string");
            }
          buffer = newbuf;
        }

      buffer[length++] = c;
      advance (lexer);
    }

  if (peek (lexer) != '"')
    panic (lexer, "unterminated string literal");

  buffer[length] = '\0';
  advance (lexer); // skip closing quote

  return create_token_with_value (lexer, TOKEN_STRING, buffer);
}

static Token
number_token (Lexer *lexer)
{
  size_t number_start = lexer->position;
  bool is_float = false;

  if (peek (lexer) == '-')
    advance (lexer);

  while (isdigit (peek (lexer)) || peek (lexer) == '.')
    {
      if (peek (lexer) == '.')
        {
          if (is_float)
            panic (lexer, "multiple '.' in number");
          is_float = true;
        }
      advance (lexer);
    }

  size_t number_end = lexer->position;
  char *number = copy_substring (lexer->source, number_start, number_end);

  Token_Type type = is_float ? TOKEN_FLOAT : TOKEN_INTEGER;
  return create_token_with_value (lexer, type, number);
}

static Token
symbol_token (Lexer *lexer)
{
  size_t symbol_start = lexer->position;

  while (is_allowed_for_symbol (peek (lexer)))
    advance (lexer);

  size_t symbol_end = lexer->position;
  char *symbol = copy_substring (lexer->source, symbol_start, symbol_end);

  return create_token_with_value (lexer, TOKEN_SYMBOL, symbol);
}

// ───────────────────────────────
// Character classification
// ───────────────────────────────

static bool
is_allowed_for_symbol (char ch)
{
  if (ch == '\0')
    return false;
  const char *not_allowed = " \t\n\r()[]\";,\'|\\#";
  return strchr (not_allowed, ch) == NULL;
}
