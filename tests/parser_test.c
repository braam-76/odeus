#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/lexer.h"
#include "core/parser.h"

#include "utest.h"

static Lexer lexer;
static Parser *parser;

UTEST(parser, init)
{
    char *source = "(print t \"Hello, world\" (+ 1 0.2))";
    lexer = lexer_from_string(source, strlen(source));
    parser = parser_init(&lexer);

    ASSERT_TRUE(parser != NULL);
    ASSERT_TRUE(parser->start_node != NULL);
    ASSERT_TRUE(parser->start_node->as.CONS.CDR == nil());
}

UTEST(parser, parse_program)
{
    parser_parse(parser);

    Val *program = parser->start_node->as.CONS.CDR;
    ASSERT_TRUE(program != nil());

    // Check first top-level expression
    Val *first_expr = program->as.CONS.CAR;
    ASSERT_EQ(first_expr->type, VALUE_CONS);

    Val *car = first_expr->as.CONS.CAR;
    ASSERT_EQ(car->type, VALUE_SYMBOL);
    ASSERT_STREQ("print", car->as.SYMBOL);

    Val *cdr = first_expr->as.CONS.CDR;
    ASSERT_EQ(cdr->type, VALUE_CONS);

    Val *first_arg = cdr->as.CONS.CAR;
    ASSERT_EQ(first_arg->type, VALUE_SYMBOL);
    ASSERT_STREQ("t", first_arg->as.SYMBOL);

    Val *second_arg_cdr = cdr->as.CONS.CDR;
    ASSERT_EQ(second_arg_cdr->type, VALUE_CONS);
    Val *second_arg = second_arg_cdr->as.CONS.CAR;
    ASSERT_EQ(second_arg->type, VALUE_STRING);
    ASSERT_STREQ("Hello, world", second_arg->as.STRING);

    Val *third_arg_cdr = second_arg_cdr->as.CONS.CDR;
    ASSERT_EQ(third_arg_cdr->type, VALUE_CONS);
    Val *third_arg = third_arg_cdr->as.CONS.CAR;
    ASSERT_EQ(third_arg->type, VALUE_CONS);

    // Nested + expression
    Val *plus_expr = third_arg->as.CONS.CAR;
    ASSERT_EQ(plus_expr->type, VALUE_SYMBOL);
    ASSERT_STREQ("+", plus_expr->as.SYMBOL);

    Val *plus_args = third_arg->as.CONS.CDR;
    ASSERT_EQ(plus_args->type, VALUE_CONS);

    Val *plus_first = plus_args->as.CONS.CAR;
    ASSERT_EQ(plus_first->type, VALUE_INTEGER);
    ASSERT_EQ(1, plus_first->as.INTEGER);

    Val *plus_second_cdr = plus_args->as.CONS.CDR;
    ASSERT_EQ(plus_second_cdr->type, VALUE_CONS);
    Val *plus_second = plus_second_cdr->as.CONS.CAR;
    ASSERT_EQ(plus_second->type, VALUE_FLOAT);
    ASSERT_EQ(0.2, plus_second->as.FLOAT);
}

UTEST(parser, walk_program)
{
    parser_parse(parser);

    Val *program = parser->start_node->as.CONS.CDR;
    size_t count = 0;

    while (program && program != nil())
    {
        Val *expr = program->as.CONS.CAR;
        printf("Expr %zu: ", count);
        value_print(expr);
        printf("\n");

        program = program->as.CONS.CDR;
        count++;
    }

    ASSERT_TRUE(count > 0);
}


UTEST_STATE();

int main(int argc, const char *const argv[])
{
    return utest_main(argc, argv);
}
