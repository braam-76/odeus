#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../core/odeus_lexer.h"
#include "../core/odeus_parser.h"

#include "utest.h"

static Lexer lexer;
static Parser *parser;

UTEST(odeus_parser, init)
{
    char *source = "(print t \"Hello, world\" (+ 1 0.2))";
    lexer = lexer_from_string(source, strlen(source));
    parser = parser_init(&lexer);

    ASSERT_TRUE(parser != NULL);
    ASSERT_TRUE(parser->start_node != NULL);
    ASSERT_TRUE(parser->start_node->as.CONS.CDR == nil());
}

UTEST(odeus_parser, parse_program)
{
    parser_parse(parser);

    AST *program = parser->start_node->as.CONS.CDR;
    ASSERT_TRUE(program != nil());

    // Check first top-level expression
    AST *first_expr = program->as.CONS.CAR;
    ASSERT_EQ(first_expr->type, AST_CONS);

    AST *car = first_expr->as.CONS.CAR;
    ASSERT_EQ(car->type, AST_SYMBOL);
    ASSERT_STREQ("print", car->as.SYMBOL);

    AST *cdr = first_expr->as.CONS.CDR;
    ASSERT_EQ(cdr->type, AST_CONS);

    AST *first_arg = cdr->as.CONS.CAR;
    ASSERT_EQ(first_arg->type, AST_SYMBOL);
    ASSERT_STREQ("t", first_arg->as.SYMBOL);

    AST *second_arg_cdr = cdr->as.CONS.CDR;
    ASSERT_EQ(second_arg_cdr->type, AST_CONS);
    AST *second_arg = second_arg_cdr->as.CONS.CAR;
    ASSERT_EQ(second_arg->type, AST_STRING);
    ASSERT_STREQ("Hello, world", second_arg->as.STRING);

    AST *third_arg_cdr = second_arg_cdr->as.CONS.CDR;
    ASSERT_EQ(third_arg_cdr->type, AST_CONS);
    AST *third_arg = third_arg_cdr->as.CONS.CAR;
    ASSERT_EQ(third_arg->type, AST_CONS);

    // Nested + expression
    AST *plus_expr = third_arg->as.CONS.CAR;
    ASSERT_EQ(plus_expr->type, AST_SYMBOL);
    ASSERT_STREQ("+", plus_expr->as.SYMBOL);

    AST *plus_args = third_arg->as.CONS.CDR;
    ASSERT_EQ(plus_args->type, AST_CONS);

    AST *plus_first = plus_args->as.CONS.CAR;
    ASSERT_EQ(plus_first->type, AST_INTEGER);
    ASSERT_EQ(1, plus_first->as.INTEGER);

    AST *plus_second_cdr = plus_args->as.CONS.CDR;
    ASSERT_EQ(plus_second_cdr->type, AST_CONS);
    AST *plus_second = plus_second_cdr->as.CONS.CAR;
    ASSERT_EQ(plus_second->type, AST_FLOAT);
    ASSERT_EQ(0.2, plus_second->as.FLOAT);
}

UTEST(odeus_parser, walk_program)
{
    parser_parse(parser);

    AST *program = parser->start_node->as.CONS.CDR;
    size_t count = 0;

    while (program && program != nil())
    {
        AST *expr = program->as.CONS.CAR;
        printf("Expr %zu: ", count);
        ast_print(expr);
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
