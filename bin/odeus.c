#include <gc/gc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "builtins/set_builtins.h"
#include "core/environment.h"
#include "core/eval.h"
#include "core/lexer.h"
#include "core/parser.h"
#include "core/symbol_map.h"
#include "core/value.h"

char *
read_file_to_string (const char *filename)
{
  FILE *f = fopen (filename, "rb");
  if (!f)
    return NULL;

  fseek (f, 0, SEEK_END);
  long size = ftell (f);
  fseek (f, 0, SEEK_SET);

  char *buffer = GC_malloc (size + 1);
  if (!buffer)
    {
      fclose (f);
      return NULL;
    }

  fread (buffer, 1, size, f);
  buffer[size] = '\0';
  fclose (f);

  return buffer;
}

int
main (int argc, char **argv)
{
  GC_INIT ();
  GC_enable_incremental ();

  symbol_map_init();
  // Persistent global environment
  Environment *global_env = env_init (NULL);
  set_builtins (global_env);

  if (argc > 1)
    {
      char *filename = argv[1];
      char *file_content = read_file_to_string (filename);
      if (!file_content)
        {
          fprintf (stderr, "Failed to open file: %s\n", filename);
          return 1;
        }

      Lexer lexer
          = lexer_from_file (filename, file_content, strlen (file_content));
      Parser *parser = parser_init (&lexer);
      AST *program = parser_parse (parser);
      Value *lower = val_from_ast (program);

      evaluate_expression (global_env, lower);
    }
  else
    {
      // REPL mode
      char *prompt = "odeus> ";
      Value *result = NULL;
      GC_add_roots (&result, &result + 1);

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

          Lexer lexer = lexer_from_string (input, strlen (input));
          Parser *parser = parser_init (&lexer);
          AST *program = parser_parse (parser);
          Value *lower = val_from_ast (program);

          result = evaluate_expression (global_env, lower);
          printf ("-> ");
          value_print (result);
          printf ("\n");

          free (input);

          static int counter = 0;
          if (++counter % 100 == 0)
            GC_gcollect ();
        }
      GC_remove_roots (&result, &result + 1);
    }

  return 0;
}
