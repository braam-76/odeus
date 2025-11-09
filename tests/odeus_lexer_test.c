#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../core/odeus_lexer.h"
#include "../core/odeus_token.h"
#include "utest.h"

static core_Lexer lexer;

UTEST(odeus_lexer, init)
{
    const char *filename = "tests/odeus_lexer_test.ode";
    FILE *f = fopen(filename, "r");
    ASSERT_TRUE(f != NULL);

    fseek(f, 0, SEEK_END);
    size_t source_size = ftell(f);
    rewind(f);

    char *source = malloc(source_size + 1);
    ASSERT_TRUE(source != NULL);

    size_t bytes_read = fread(source, 1, source_size, f);
    source[bytes_read] = '\0';
    fclose(f);

    lexer = core_lexer_init(filename, source, source_size);

    ASSERT_EQ(filename, lexer.filename);
    ASSERT_EQ(source, lexer.source);
    ASSERT_EQ(source_size, lexer.source_size);
}

UTEST(odeus_lexer, parenthesis)
{
    core_Token open_paren = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_OPEN_PAREN, (int)open_paren.type);
    ASSERT_EQ((size_t)2, open_paren.line);

    core_Token close_paren = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_CLOSE_PAREN, (int)close_paren.type);
    ASSERT_EQ((size_t)4, close_paren.line);
}

UTEST(odeus_lexer, strings)
{
    core_Token string = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_STRING, (int)string.type);
    ASSERT_STREQ(string.value, "string");
    ASSERT_EQ((size_t)6, string.line);

    core_Token string_with_escapes = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_STRING, (int)string_with_escapes.type);
    ASSERT_STREQ(string_with_escapes.value, "string with escapes: \" \n \t \r '");
    ASSERT_EQ((size_t)8, string_with_escapes.line);
}

UTEST(odeus_lexer, numbers)
{
    core_Token positive_number = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_INTEGER, (int)positive_number.type);
    ASSERT_STREQ(positive_number.value, "1");
    ASSERT_EQ((size_t)11, positive_number.line);

    core_Token negative_integer = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_INTEGER, (int)negative_integer.type);
    ASSERT_STREQ(negative_integer.value, "-1");
    ASSERT_EQ((size_t)12, negative_integer.line);

    core_Token positive_float = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_FLOAT, (int)positive_float.type);
    ASSERT_STREQ(positive_float.value, "0.1");
    ASSERT_EQ((size_t)13, positive_float.line);

    core_Token negative_float = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_FLOAT, (int)negative_float.type);
    ASSERT_STREQ(negative_float.value, "-0.1");
    ASSERT_EQ((size_t)14, negative_float.line);
}

UTEST (odeus_lexer, atoms)
{
    core_Token atom = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_ATOM, (int)atom.type);
    ASSERT_STREQ(atom.value, "atom");
    ASSERT_EQ((size_t)18, atom.line);

    core_Token starred_atom = core_lexer_next_token(&lexer);
    ASSERT_EQ (TOKEN_ATOM, (int)starred_atom.type);
    ASSERT_STREQ(starred_atom.value, "*atom*");
    ASSERT_EQ((size_t)19, starred_atom.line);

    core_Token all_characters_atom = core_lexer_next_token(&lexer);
    ASSERT_EQ (TOKEN_ATOM, (int)all_characters_atom.type);
    ASSERT_STREQ(all_characters_atom.value, "abcdefghijklmnoprstqwxyz_ABCDEFGHIJKLMNOPRSTQWXYZ-0123456789/:.*+_=-!?<>@$%^&");
    ASSERT_EQ((size_t)20, all_characters_atom.line);
}

UTEST (odeus_lexer, quote_before_parens)
{
    core_Token quote = core_lexer_next_token(&lexer);
    ASSERT_EQ (TOKEN_QUOTE, (int)quote.type);
    ASSERT_EQ ((size_t)23, quote.line);

    core_Token open_paren = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_OPEN_PAREN, (int)open_paren.type);
    ASSERT_EQ((size_t)23, open_paren.line);

    core_Token close_paren = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_CLOSE_PAREN, (int)close_paren.type);
    ASSERT_EQ((size_t)24, close_paren.line);
}

UTEST (odeus_lexer, quote_before_atom)
{
    core_Token quote = core_lexer_next_token(&lexer);
    ASSERT_EQ (TOKEN_QUOTE, (int)quote.type);
    ASSERT_EQ ((size_t)25, quote.line);

    core_Token atom = core_lexer_next_token(&lexer);
    ASSERT_EQ(TOKEN_ATOM, (int)atom.type);
    ASSERT_STREQ(atom.value, "quoted-atom");
    ASSERT_EQ((size_t)25, atom.line);
}

UTEST (odeus_lexer, end_of_file)
{
    core_Token end_of_file = core_lexer_next_token(&lexer);
    ASSERT_EQ (TOKEN_END_OF_FILE, (int)end_of_file.type);
    ASSERT_EQ ((size_t)26, end_of_file.line);
}

UTEST_MAIN()
