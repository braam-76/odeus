#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/lexer.h"
#include "core/token.h"
#include "utest.h"

static Lexer lexer;
char *filename = NULL;

UTEST (lexer, init)
{
  FILE *f = fopen (filename, "r");
  if (!f)
    {
      fprintf (stderr, "ERROR: utest_lexer_init: could not open file: %s\n", strerror (errno));
      exit (1);
    }

  fseek (f, 0, SEEK_END);
  size_t source_size = ftell (f);
  rewind (f);

  char *source = malloc (source_size + 1);
  ASSERT_TRUE (source != NULL);

  size_t bytes_read = fread (source, 1, source_size, f);
  source[bytes_read] = '\0';
  fclose (f);

  lexer = lexer_from_file (filename, source, source_size);

  ASSERT_EQ (filename, lexer.filename);
  ASSERT_EQ (source, lexer.source);
  ASSERT_EQ (source_size, lexer.source_size);
}

UTEST (lexer, parenthesis)
{
  Token open_paren = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_OPEN_PAREN, (int)open_paren.type);
  ASSERT_EQ ((size_t)2, open_paren.line);
  ASSERT_EQ ((size_t)0, open_paren.column);

  Token close_paren = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_CLOSE_PAREN, (int)close_paren.type);
  ASSERT_EQ ((size_t)4, close_paren.line);
  ASSERT_EQ ((size_t)3, close_paren.column);
}

UTEST (lexer, strings)
{
  Token string = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_STRING, (int)string.type);
  ASSERT_STREQ (string.value, "string");
  ASSERT_EQ ((size_t)6, string.line);
  ASSERT_EQ ((size_t)7, string.column);

  Token string_with_escapes = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_STRING, (int)string_with_escapes.type);
  ASSERT_STREQ (string_with_escapes.value, "string with escapes: \" \n \t \r '");
  ASSERT_EQ ((size_t)8, string_with_escapes.line);
  ASSERT_EQ ((size_t)0, string_with_escapes.column);
}

UTEST (lexer, numbers)
{
  Token positive_number = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_INTEGER, (int)positive_number.type);
  ASSERT_STREQ (positive_number.value, "1");
  ASSERT_EQ ((size_t)11, positive_number.line);
  ASSERT_EQ ((size_t)0, positive_number.column);

  Token negative_integer = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_INTEGER, (int)negative_integer.type);
  ASSERT_STREQ (negative_integer.value, "-1");
  ASSERT_EQ ((size_t)12, negative_integer.line);
  ASSERT_EQ ((size_t)0, negative_integer.column);

  Token positive_float = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_FLOAT, (int)positive_float.type);
  ASSERT_STREQ (positive_float.value, "0.1");
  ASSERT_EQ ((size_t)13, positive_float.line);
  ASSERT_EQ ((size_t)0, positive_float.column);

  Token negative_float = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_FLOAT, (int)negative_float.type);
  ASSERT_STREQ (negative_float.value, "-0.1");
  ASSERT_EQ ((size_t)14, negative_float.line);
  ASSERT_EQ ((size_t)0, negative_float.column);
}
UTEST (lexer, symbols)
{
  Token symbol = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_SYMBOL, (int)symbol.type);
  ASSERT_STREQ (symbol.value, "symbol");
  ASSERT_EQ ((size_t)18, symbol.line);
  ASSERT_EQ ((size_t)0, symbol.column);

  Token starred_symbol = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_SYMBOL, (int)starred_symbol.type);
  ASSERT_STREQ (starred_symbol.value, "*symbol*");
  ASSERT_EQ ((size_t)19, starred_symbol.line);
  ASSERT_EQ ((size_t)100, starred_symbol.column);

  Token all_characters_symbol = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_SYMBOL, (int)all_characters_symbol.type);
  ASSERT_STREQ (all_characters_symbol.value,
                "abcdefghijklmnoprstqwxyz_ABCDEFGHIJKLMNOPRSTQWXYZ-0123456789/:.*+_=-!?<>@$%^&");
  ASSERT_EQ ((size_t)20, all_characters_symbol.line);
  ASSERT_EQ ((size_t)0, all_characters_symbol.column);
}

UTEST (lexer, quote_before_parens)
{
  Token quote = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_QUOTE, (int)quote.type);
  ASSERT_EQ ((size_t)23, quote.line);
  ASSERT_EQ ((size_t)0, quote.column);

  Token open_paren = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_OPEN_PAREN, (int)open_paren.type);
  ASSERT_EQ ((size_t)23, open_paren.line);
  ASSERT_EQ ((size_t)1, open_paren.column);

  Token close_paren = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_CLOSE_PAREN, (int)close_paren.type);
  ASSERT_EQ ((size_t)24, close_paren.line);
  ASSERT_EQ ((size_t)0, close_paren.column);
}

UTEST (lexer, quote_before_symbol)
{
  Token quote = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_QUOTE, (int)quote.type);
  ASSERT_EQ ((size_t)25, quote.line);
  ASSERT_EQ ((size_t)0, quote.column);

  Token symbol = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_SYMBOL, (int)symbol.type);
  ASSERT_STREQ (symbol.value, "quoted-symbol");
  ASSERT_EQ ((size_t)25, symbol.line);
  ASSERT_EQ ((size_t)1, symbol.column);
}
UTEST (lexer, end_of_file)
{
  Token end_of_file = lexer_next_token (&lexer);
  ASSERT_EQ (TOKEN_END_OF_FILE, (int)end_of_file.type);
  ASSERT_EQ ((size_t)26, end_of_file.line);
  ASSERT_EQ ((size_t)0, end_of_file.column);
}

UTEST_STATE ();

int
main (int argc, const char *const argv[])
{
  filename = argc == 2 ? argv[1] : "lexer_test.ode";
  return utest_main (argc, argv);
}
