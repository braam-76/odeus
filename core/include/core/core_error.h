#ifndef CORE_ERROR_H_
#define CORE_ERROR_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
  OK = 0,
  ERROR
} Error_Status;

typedef struct
{
  Error_Status status;
  const char *message;
  const char *filename;
  long line;
  long column;
} Error;

#endif // CORE_ERROR_H_
