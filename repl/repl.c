#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "../builtins/set_builtins.h"
#include "../core/eval.h"
#include "../core/lexer.h"
#include "../core/parser.h"

int
main ()
{
  char *prompt = "odeus> ";

  // Persistent global environment
  AST *global_env = make_cons (nil (), nil ());
  set_builtins (global_env);

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

      AST *program = parser_parse (parser);

      // Evaluate the BEGIN node in CDR(parser->start_node)
      AST *result = evaluate_expression (global_env, program);

      ast_print (result);
      printf ("\n");

      ast_free (program);
      free (parser);
      free (input);
    }

  return 0;
}
