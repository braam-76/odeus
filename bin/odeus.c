#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "builtins/set_builtins.h"
#include "core/eval.h"
#include "core/lexer.h"
#include "core/parser.h"

char *
read_file_to_string (const char *filename)
{
  FILE *f = fopen (filename, "rb");
  if (!f)
    return NULL;

  fseek (f, 0, SEEK_END);
  long size = ftell (f);
  fseek (f, 0, SEEK_SET);

  char *buffer = malloc (size + 1);
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
  // Persistent global environment
  Val *global_env = val_cons (nil (), nil ());
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
      Val *program = parser_parse (parser);

      evaluate_expression (global_env, program);

      free (parser);
      free (file_content);
    }
  else
    {
      // REPL mode
      char *prompt = "odeus> ";
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
          Val *program = parser_parse (parser);

          Val *result = evaluate_expression (global_env, program);
          printf ("-> ");
          value_print (result);
          printf ("\n");

          free (parser);
          free (input);
        }
    }

  return 0;
}
