#include "core/ast.h"

#include <stdarg.h> // For va_list

/* ----------------------------------------------------------
 *   STRING CONVERSION (for write function)
 * ---------------------------------------------------------- */

// Helper: Append formatted string to dynamic buffer
static void
append_string (char **buffer, size_t *capacity, size_t *length, const char *format, ...)
{
  va_list args;
  va_start (args, format);

  // Get the required size
  va_list args_copy;
  va_copy (args_copy, args);
  int needed = vsnprintf (NULL, 0, format, args_copy);
  va_end (args_copy);

  if (needed < 0)
    {
      va_end (args);
      return;
    }

  // Ensure we have enough space
  if (*length + needed + 1 > *capacity)
    {
      size_t new_capacity = *capacity * 2 + needed + 1;
      char *new_buffer = realloc (*buffer, new_capacity);
      if (!new_buffer)
        {
          va_end (args);
          return;
        }
      *buffer = new_buffer;
      *capacity = new_capacity;
    }

  // Append the formatted string
  *length += vsnprintf (*buffer + *length, *capacity - *length, format, args);
  va_end (args);
}

// Helper: Escape special characters in strings
static char *
escape_string (const char *str)
{
  // Count how many characters need escaping
  size_t len = strlen (str);
  size_t escaped_len = len + 2; // Start with quotes

  for (size_t i = 0; i < len; i++)
    {
      char c = str[i];
      if (c == '"' || c == '\\' || c == '\n' || c == '\t' || c == '\r')
        escaped_len++; // Need backslash for these
    }

  char *escaped = malloc (escaped_len + 1);
  if (!escaped)
    return NULL;

  char *dest = escaped;
  *dest++ = '"';

  for (size_t i = 0; i < len; i++)
    {
      char c = str[i];
      switch (c)
        {
        case '"':
          *dest++ = '\\';
          *dest++ = '"';
          break;
        case '\\':
          *dest++ = '\\';
          *dest++ = '\\';
          break;
        case '\n':
          *dest++ = '\\';
          *dest++ = 'n';
          break;
        case '\t':
          *dest++ = '\\';
          *dest++ = 't';
          break;
        case '\r':
          *dest++ = '\\';
          *dest++ = 'r';
          break;
        default: *dest++ = c; break;
        }
    }

  *dest++ = '"';
  *dest = '\0';

  return escaped;
}

// Recursive function to convert AST to string
static void
ast_to_string_recursive (AST *node, char **buffer, size_t *capacity, size_t *length)
{
  if (!node)
    {
      append_string (buffer, capacity, length, "()");
      return;
    }

  switch (node->type)
    {
    case AST_NIL: append_string (buffer, capacity, length, "nil"); break;

    case AST_SYMBOL: append_string (buffer, capacity, length, "%s", node->as.SYMBOL); break;

    case AST_INTEGER: append_string (buffer, capacity, length, "%ld", node->as.INTEGER); break;

    case AST_FLOAT:
      {
        double value = node->as.FLOAT;
        if (value == (long)value)
          append_string (buffer, capacity, length, "%.1f", value);
        else
          append_string (buffer, capacity, length, "%.10g", value);
      }
      break;

    case AST_STRING:
      {
        // Strings need quotes and escaping
        char *escaped = escape_string (node->as.STRING);
        if (escaped)
          {
            append_string (buffer, capacity, length, "%s", escaped);
            free (escaped);
          }
      }
      break;

    case AST_QUOTE:
      append_string (buffer, capacity, length, "'");
      ast_to_string_recursive (node->as.QUOTE.EXPR, buffer, capacity, length);
      break;

    case AST_CONS:
      {
        append_string (buffer, capacity, length, "(");
        AST *cur = node;

        while (cur->type == AST_CONS)
          {
            ast_to_string_recursive (CAR (cur), buffer, capacity, length);
            cur = CDR (cur);

            if (cur->type == AST_CONS)
              append_string (buffer, capacity, length, " ");
          }

        if (cur->type != AST_NIL)
          {
            append_string (buffer, capacity, length, " . ");
            ast_to_string_recursive (cur, buffer, capacity, length);
          }
        append_string (buffer, capacity, length, ")");
        break;
      }

    case AST_BUILTIN_NORMAL: append_string (buffer, capacity, length, "#<builtin function>"); break;

    case AST_BUILTIN_SPECIAL: append_string (buffer, capacity, length, "#<special form>"); break;

    case AST_ERROR: append_string (buffer, capacity, length, "%s", node->as.ERROR.MESSAGE); break;

    case AST_END_OF_FILE: append_string (buffer, capacity, length, "#<EOF>"); break;

    default: append_string (buffer, capacity, length, "#<UNKNOWN:%d>", node->type); break;
    }
}

// Main function to convert AST to string
char *
ast_to_string (AST *node)
{
  if (!node)
    return strdup ("()");

  size_t capacity = 128; // Initial capacity
  size_t length = 0;
  char *buffer = malloc (capacity);

  if (!buffer)
    return NULL;

  buffer[0] = '\0';

  ast_to_string_recursive (node, &buffer, &capacity, &length);

  // Shrink to fit
  char *result = realloc (buffer, length + 1);
  return result ? result : buffer;
}
