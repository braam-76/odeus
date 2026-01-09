#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "../core/lexer.h"
#include "../core/parser.h"
#include "../core/eval.h"
#include "../core/builtins/set_builtins.h"
int
main ()
{
  char *prompt = "odeus> ";

  // Persistent global environment
  AST *global_env = make_cons (nil (), nil ());
  set_builtins(global_env);

  while (1)
    {
      char *input = readline (prompt);
      if (!input)
        {
          printf ("\n");
          break;
        }

      if (input[0] != '\0')
        add_history (input);

      // Parse the input
      Lexer lexer = lexer_from_string (input, strlen (input));
      Parser *parser = parser_init (&lexer);

      // **Override parser start_node to reuse existing global_env**
      // program will be built in CDR(parser->start_node)
      // (I know, so hackish to do it in main(), but I will fix it later)
      parser->start_node->as.CONS.CAR = global_env;

      parser_parse (parser);

      // Evaluate the BEGIN node in CDR(parser->start_node)
      AST *result = evaluate_expression (global_env, CDR (parser->start_node));

      ast_print (result);
      printf ("\n");

      free (parser);
      free (input);
    }

  return 0;
}
