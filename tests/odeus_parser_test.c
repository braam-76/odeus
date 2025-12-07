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
    ASSERT_TRUE(parser->start_node->value.CONS.CDR == nil());
}

UTEST(odeus_parser, parse_program)
{
    parser_parse(parser);

    AST_Node *program = parser->start_node->value.CONS.CDR;
    ASSERT_TRUE(program != nil());

    // Check first top-level expression
    AST_Node *first_expr = program->value.CONS.CAR;
    ASSERT_EQ(first_expr->type, AST_CONS);

    AST_Node *car = first_expr->value.CONS.CAR;
    ASSERT_EQ(car->type, AST_SYMBOL);
    ASSERT_STREQ("print", car->value.SYMBOL);

    AST_Node *cdr = first_expr->value.CONS.CDR;
    ASSERT_EQ(cdr->type, AST_CONS);

    AST_Node *first_arg = cdr->value.CONS.CAR;
    ASSERT_EQ(first_arg->type, AST_SYMBOL);
    ASSERT_STREQ("t", first_arg->value.SYMBOL);

    AST_Node *second_arg_cdr = cdr->value.CONS.CDR;
    ASSERT_EQ(second_arg_cdr->type, AST_CONS);
    AST_Node *second_arg = second_arg_cdr->value.CONS.CAR;
    ASSERT_EQ(second_arg->type, AST_STRING);
    ASSERT_STREQ("Hello, world", second_arg->value.STRING);

    AST_Node *third_arg_cdr = second_arg_cdr->value.CONS.CDR;
    ASSERT_EQ(third_arg_cdr->type, AST_CONS);
    AST_Node *third_arg = third_arg_cdr->value.CONS.CAR;
    ASSERT_EQ(third_arg->type, AST_CONS);

    // Nested + expression
    AST_Node *plus_expr = third_arg->value.CONS.CAR;
    ASSERT_EQ(plus_expr->type, AST_SYMBOL);
    ASSERT_STREQ("+", plus_expr->value.SYMBOL);

    AST_Node *plus_args = third_arg->value.CONS.CDR;
    ASSERT_EQ(plus_args->type, AST_CONS);

    AST_Node *plus_first = plus_args->value.CONS.CAR;
    ASSERT_EQ(plus_first->type, AST_INTEGER);
    ASSERT_EQ(1, plus_first->value.INTEGER);

    AST_Node *plus_second_cdr = plus_args->value.CONS.CDR;
    ASSERT_EQ(plus_second_cdr->type, AST_CONS);
    AST_Node *plus_second = plus_second_cdr->value.CONS.CAR;
    ASSERT_EQ(plus_second->type, AST_FLOAT);
    ASSERT_EQ(0.2, plus_second->value.FLOAT);
}

UTEST(odeus_parser, walk_program)
{
    parser_parse(parser);

    AST_Node *program = parser->start_node->value.CONS.CDR;
    size_t count = 0;

    while (program && program != nil())
    {
        AST_Node *expr = program->value.CONS.CAR;
        printf("Expr %zu: ", count);
        ast_print(expr);
        printf("\n");

        program = program->value.CONS.CDR;
        count++;
    }

    ASSERT_TRUE(count > 0);
}


UTEST_STATE();

int main(int argc, const char *const argv[])
{
    return utest_main(argc, argv);
}
