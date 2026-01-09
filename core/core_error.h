#ifndef ODEUS_CORE_ERROR_H_
#define ODEUS_CORE_ERROR_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ODEUS_OK = 0,
    ODEUS_ERROR
} Error_Status;

typedef struct {
    Error_Status status;
    const char *message;
    const char *filename;
    long line;
    long column;
} Error;

#endif // ODEUS_CORE_ERROR_H_
