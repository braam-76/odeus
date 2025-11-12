#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../core/odeus_lexer.h"
#include "../core/odeus_parser.h"
#include "../core/odeus_token.h"

#include "utest.h"

static Lexer lexer;
static Parser *parser;
char *filename = NULL;

UTEST(odeus_parser, init)
{
  FILE *f = fopen(filename, "r");

  if (!f)
  {
    fprintf(stderr, "ERROR: utest_odeus_lexer_init: could not open file: %s\n",
            strerror(errno));
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  size_t source_size = ftell(f);
  rewind(f);

  char *source = malloc(source_size + 1);
  ASSERT_TRUE(source != NULL);

  size_t bytes_read = fread(source, 1, source_size, f);
  source[bytes_read] = '\0';
  fclose(f);

  lexer = lexer_init(filename, source, source_size);

  ASSERT_EQ(filename, lexer.filename);
  ASSERT_EQ(source, lexer.source);
  ASSERT_EQ(source_size, lexer.source_size);

  parser = parser_init(&lexer);

  ASSERT_EQ(filename, parser->lexer->filename);
}

UTEST(odeus_parser, parsed_whole_file)
{
  parser_parse(parser);

  ASSERT_EQ(parser->file->type, (unsigned)AST_FILE);
  ASSERT_TRUE(parser->file->value.FILE.COUNT > 0);
}

UTEST(odeus_parser, first_cons)
{
  // First expression: (print t "Hello, world" (+ 1 0.2))
  AST_Node *first_expr = parser->file->value.FILE.BODY[0];
  ASSERT_EQ(first_expr->type, AST_CONS);

  // CAR should be "print" atom
  AST_Node *car = first_expr->value.CONS.CAR;
  ASSERT_EQ(car->type, AST_ATOM);
  ASSERT_STREQ("print", car->value.ATOM);

  // CDR should be a list of arguments
  AST_Node *cdr = first_expr->value.CONS.CDR;
  ASSERT_EQ(cdr->type, AST_CONS);

  // First argument: t (atom)
  AST_Node *first_arg = cdr->value.CONS.CAR;
  ASSERT_EQ(first_arg->type, AST_ATOM);
  ASSERT_STREQ("t", first_arg->value.ATOM);

  // Second argument: "Hello, world" (string)
  AST_Node *second_arg_cdr = cdr->value.CONS.CDR;
  ASSERT_EQ(second_arg_cdr->type, AST_CONS);
  AST_Node *second_arg = second_arg_cdr->value.CONS.CAR;
  ASSERT_EQ(second_arg->type, AST_STRING);
  ASSERT_STREQ("Hello, world", second_arg->value.STRING);

  // Third argument: (+ 1 0.2) (nested list)
  AST_Node *third_arg_cdr = second_arg_cdr->value.CONS.CDR;
  ASSERT_EQ(third_arg_cdr->type, AST_CONS);
  AST_Node *third_arg = third_arg_cdr->value.CONS.CAR;
  ASSERT_EQ(third_arg->type, AST_CONS);

  // Verify the nested expression structure
  AST_Node *plus_expr = third_arg->value.CONS.CAR;
  ASSERT_EQ(plus_expr->type, AST_ATOM);
  ASSERT_STREQ("+", plus_expr->value.ATOM);

  AST_Node *plus_args = third_arg->value.CONS.CDR;
  ASSERT_EQ(plus_args->type, AST_CONS);

  // First argument to +: 1 (integer)
  AST_Node *plus_first_arg = plus_args->value.CONS.CAR;
  ASSERT_EQ(plus_first_arg->type, AST_INTEGER);
  ASSERT_EQ(1, plus_first_arg->value.INTEGER);

  // Second argument to +: 0.2 (float)
  AST_Node *plus_second_arg_cdr = plus_args->value.CONS.CDR;
  ASSERT_EQ(plus_second_arg_cdr->type, AST_CONS);
  AST_Node *plus_second_arg = plus_second_arg_cdr->value.CONS.CAR;

  ASSERT_EQ(plus_second_arg->type, AST_FLOAT);
  ASSERT_EQ(0.2, plus_second_arg->value.FLOAT);
}

UTEST(odeus_parser, second_cons)
{
  // Second expression: (def something (arg1 arg2) (print '(+ arg1 arg2)))
  AST_Node *second_expr = parser->file->value.FILE.BODY[1];
  ASSERT_EQ(second_expr->type, AST_CONS);

  // CAR should be "def" atom
  AST_Node *car = second_expr->value.CONS.CAR;
  ASSERT_EQ(car->type, AST_ATOM);
  ASSERT_STREQ("def", car->value.ATOM);

  // CDR should contain function definition parts
  AST_Node *cdr = second_expr->value.CONS.CDR;
  ASSERT_EQ(cdr->type, AST_CONS);

  // Function name: something
  AST_Node *func_name = cdr->value.CONS.CAR;
  ASSERT_EQ(func_name->type, AST_ATOM);
  ASSERT_STREQ("something", func_name->value.ATOM);

  // Parameter list
  AST_Node *params_cdr = cdr->value.CONS.CDR;
  ASSERT_EQ(params_cdr->type, AST_CONS);
  AST_Node *params = params_cdr->value.CONS.CAR;
  ASSERT_EQ(params->type, AST_CONS);

  // First parameter: arg1
  AST_Node *first_param = params->value.CONS.CAR;
  ASSERT_EQ(first_param->type, AST_ATOM);
  ASSERT_STREQ("arg1", first_param->value.ATOM);

  // Second parameter: arg2
  AST_Node *second_param_cdr = params->value.CONS.CDR;
  ASSERT_EQ(second_param_cdr->type, AST_CONS);
  AST_Node *second_param = second_param_cdr->value.CONS.CAR;
  ASSERT_EQ(second_param->type, AST_ATOM);
  ASSERT_STREQ("arg2", second_param->value.ATOM);

  // Function body
  AST_Node *body_cdr = params_cdr->value.CONS.CDR;
  ASSERT_EQ(body_cdr->type, AST_CONS);
  AST_Node *body = body_cdr->value.CONS.CAR;
  ASSERT_EQ(body->type, AST_CONS);

  // Body is a print statement with quoted expression
  AST_Node *body_car = body->value.CONS.CAR;
  ASSERT_EQ(body_car->type, AST_ATOM);
  ASSERT_STREQ("print", body_car->value.ATOM);

  body_cdr = body->value.CONS.CDR;
  ASSERT_EQ(body_cdr->type, AST_CONS);
  AST_Node *quoted_expr = body_cdr->value.CONS.CAR;
  ASSERT_EQ(quoted_expr->type, AST_QUOTE);

  // Verify the quoted expression
  AST_Node *quoted_cons = quoted_expr->value.QUOTE.EXPR;
  ASSERT_EQ(quoted_cons->type, AST_CONS);

  AST_Node *quoted_plus = quoted_cons->value.CONS.CAR;
  ASSERT_EQ(quoted_plus->type, AST_ATOM);
  ASSERT_STREQ("+", quoted_plus->value.ATOM);

  AST_Node *quoted_args = quoted_cons->value.CONS.CDR;
  ASSERT_EQ(quoted_args->type, AST_CONS);

  AST_Node *quoted_arg1 = quoted_args->value.CONS.CAR;
  ASSERT_EQ(quoted_arg1->type, AST_ATOM);
  ASSERT_STREQ("arg1", quoted_arg1->value.ATOM);

  AST_Node *quoted_arg2_cdr = quoted_args->value.CONS.CDR;
  ASSERT_EQ(quoted_arg2_cdr->type, AST_CONS);
  AST_Node *quoted_arg2 = quoted_arg2_cdr->value.CONS.CAR;
  ASSERT_EQ(quoted_arg2->type, AST_ATOM);
  ASSERT_STREQ("arg2", quoted_arg2->value.ATOM);
}

UTEST(odeus_parser, memory_cleanup)
{
  // Clean up the parser and lexer
  if (parser && parser->file)
  {
    for (size_t i = 0; i < parser->file->value.FILE.COUNT; i++)
    {
      // In a real implementation, you'd have an ast_free function
      // to recursively free all AST nodes
      // For now, we just verify the structure is intact
      ASSERT_TRUE(parser->file->value.FILE.BODY[i] != NULL);
    }
  }

  // Note: In a complete implementation, you'd want to free:
  // - parser->file->value.FILE.BODY and all its elements
  // - parser->file
  // - parser
  // - lexer.source
  // But for unit tests, the OS will clean up anyway
}

UTEST_STATE();

int main(int argc, const char *const argv[])
{
  filename = argc == 2 ? argv[1] : "odeus_lexer_test.ode";
  return utest_main(argc, argv);
}
