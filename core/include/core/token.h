#ifndef TOKEN_H_
#define TOKEN_H_

#include <stddef.h>

typedef enum
{
  TOKEN_NONE = 0,
  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_QUOTE,
  TOKEN_PERIOD,
  TOKEN_COMMA,
  TOKEN_INTEGER,
  TOKEN_FLOAT,
  TOKEN_STRING,
  TOKEN_SYMBOL,

  TOKEN_END_OF_FILE
} Token_Type;

typedef struct
{
  Token_Type type;
  char *value;
  size_t line;
  size_t column;
  size_t position;
} Token;

#endif // TOKEN_H_
