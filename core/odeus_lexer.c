#include "odeus_lexer.h"
#include "odeus_token.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ───────────────────────────────
// Internal helpers
// ───────────────────────────────

static char peek (core_Lexer *lexer);
static char peek_next (core_Lexer *lexer, int n);
static void advance (core_Lexer *lexer);
static void panic (core_Lexer *lexer, const char *message);
static bool is_allowed_for_atom (char ch);
static char *copy_substring (const char *src, size_t start, size_t end);

static core_Token string_token (core_Lexer *lexer);
static core_Token number_token (core_Lexer *lexer);
static core_Token atom_token (core_Lexer *lexer);

// ───────────────────────────────
// Lexer initialization
// ───────────────────────────────

core_Lexer
core_lexer_init (const char *filename, const char *source, size_t source_size)
{
  core_Lexer lexer = { 0 };

  lexer.filename = filename;
  lexer.source = source;
  lexer.source_size = source_size;
  lexer.position = 0;
  lexer.line = 1;
  lexer.column = 1;

  return lexer;
}

// ───────────────────────────────
// Tokenization core
// ───────────────────────────────

core_Token
core_lexer_next_token (core_Lexer *lexer)
{
  while (true)
    {
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
        case '(': advance (lexer); return (core_Token){ .type = TOKEN_OPEN_PAREN, .line = lexer->line };
        case ')': advance (lexer); return (core_Token){ .type = TOKEN_CLOSE_PAREN, .line = lexer->line };
        case ',': advance (lexer); return (core_Token){ .type = TOKEN_COMMA, .line = lexer->line };
        case '\'': advance (lexer); return (core_Token){ .type = TOKEN_QUOTE, .line = lexer->line };

        case '.':
          if (is_allowed_for_atom (peek_next (lexer, 1))) return atom_token (lexer);
          advance (lexer);
          return (core_Token){ .type = TOKEN_PERIOD, .line = lexer->line };

        // Comments
        case ';':
          while (peek (lexer) != '\n' && peek (lexer) != '\0')
            advance (lexer);
          continue;

        // Strings
        case '"': return string_token (lexer);

        // End of file
        case '\0': return (core_Token){ .type = TOKEN_END_OF_FILE, .line = lexer->line };

        // Numbers or atoms
        default:
          if (isdigit (peek (lexer)) || (peek (lexer) == '-' && isdigit (peek_next (lexer, 1))))
            return number_token (lexer);

          else if (is_allowed_for_atom (peek (lexer)))
            return atom_token (lexer);

          else
            {
              char buf[64];
              snprintf (buf, sizeof (buf), "Unknown character: '%c'", peek (lexer));
              panic (lexer, buf);
            }
        }
    }
}

// ───────────────────────────────
// Helper functions
// ───────────────────────────────

static inline char
peek (core_Lexer *lexer)
{
  return peek_next (lexer, 0);
}

static char
peek_next (core_Lexer *lexer, int n)
{
  size_t pos = lexer->position + n;
  if (pos >= lexer->source_size) return '\0';
  return lexer->source[pos];
}

static void
advance (core_Lexer *lexer)
{
  lexer->position++;
  lexer->column++;
}

static void
panic (core_Lexer *lexer, const char *message)
{
  fprintf (stderr, "%s:%ld:%ld: ERROR: %s\n", lexer->filename, (long)lexer->line, (long)lexer->column, message);
  exit (EXIT_FAILURE);
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

// ───────────────────────────────
// Token parsing functions
// ───────────────────────────────

static core_Token
string_token (core_Lexer *lexer)
{
  advance (lexer); // skip opening quote

  size_t capacity = 64;
  size_t length = 0;
  char *buffer = malloc (capacity);
  if (!buffer) panic (lexer, "out of memory");

  while (peek (lexer) != '"' && peek (lexer) != '\0')
    {
      if (peek (lexer) == '\n') panic (lexer, "incomplete string");

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

      // Add the character
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

  if (peek (lexer) != '"') panic (lexer, "unterminated string literal");

  buffer[length] = '\0';
  advance (lexer); // skip closing quote

  return (core_Token){ .type = TOKEN_STRING, .value = buffer, .line = lexer->line };
}

static core_Token
number_token (core_Lexer *lexer)
{
  size_t number_start = lexer->position;
  bool is_float = false;

  if (peek (lexer) == '-') advance (lexer);

  while (isdigit (peek (lexer)) || peek (lexer) == '.')
    {
      if (peek (lexer) == '.')
        {
          if (is_float) panic (lexer, "multiple '.' in number");
          is_float = true;
        }
      advance (lexer);
    }

  size_t number_end = lexer->position;
  char *number = copy_substring (lexer->source, number_start, number_end);

  core_Token_Type type = is_float ? TOKEN_FLOAT : TOKEN_INTEGER;
  return (core_Token){ .type = type, .value = number, .line = lexer->line };
}

static core_Token
atom_token (core_Lexer *lexer)
{
  size_t atom_start = lexer->position;

  while (is_allowed_for_atom (peek (lexer)))
    advance (lexer);

  size_t atom_end = lexer->position;
  char *atom = copy_substring (lexer->source, atom_start, atom_end);

  return (core_Token){ .type = TOKEN_ATOM, .value = atom, .line = lexer->line };
}

// ───────────────────────────────
// Character classification
// ───────────────────────────────

static bool
is_allowed_for_atom (char ch)
{
  if (ch == '\0') return false;

  const char *not_allowed = " \t\n\r()[]\";,\'|\\#";
  return strchr(not_allowed, ch) == NULL;
}
