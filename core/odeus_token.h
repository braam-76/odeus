#ifndef ODEUS_CORE_TOKEN_H_
#define ODEUS_CORE_TOKEN_H_

// #include "odeus_utils.h"

#include <stddef.h>

#define MAX_LENGTH_NAME 255

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
  TOKEN_ATOM,
  TOKEN_END_OF_FILE
} core_Token_Type;

typedef struct
{
  core_Token_Type type;
  char *value;
  size_t line;
  size_t column;
  size_t position;
} core_Token;

static inline const char *
core_token_type_as_str (core_Token_Type token_type)
{
  switch (token_type)
    {
    case TOKEN_NONE:        return "TOKEN_NONE";
    case TOKEN_OPEN_PAREN:  return "TOKEN_OPEN_PAREN";
    case TOKEN_CLOSE_PAREN: return "TOKEN_CLOSE_PAREN";
    case TOKEN_QUOTE:       return "TOKEN_QUOTE";
    case TOKEN_PERIOD:      return "TOKEN_PERIOD";
    case TOKEN_COMMA:       return "TOKEN_COMMA";
    case TOKEN_INTEGER:     return "TOKEN_INTEGER";
    case TOKEN_FLOAT:       return "TOKEN_FLOAT";
    case TOKEN_STRING:      return "TOKEN_STRING";
    case TOKEN_ATOM:        return "TOKEN_ATOM";
    case TOKEN_END_OF_FILE: return "TOKEN_END_OF_FILE";
    default:                return "Unknown token type";
    }
}

#endif // ODEUS_CORE_TOKEN_H_
