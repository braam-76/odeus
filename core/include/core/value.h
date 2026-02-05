#ifndef VALUE_H_
#define VALUE_H_

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/ast.h"
#include "core/environment.h"
#include "core/meta.h"

typedef enum
{
  VALUE_NIL,
  VALUE_SYMBOL,
  VALUE_INTEGER,
  VALUE_FLOAT,
  VALUE_STRING,
  VALUE_CONS,

  VALUE_BUILTIN,
  VALUE_LAMBDA,
  VALUE_MACRO,

  VALUE_ERROR,
  VALUE_END_OF_FILE,
} ValueType;

typedef struct Value Val;
typedef Val *(*Builtin_Function) (Env *environment, Val *arguments);

struct Value
{
  ValueType type;
  union
  {
    long INTEGER;
    double FLOAT;
    char *STRING;
    char *SYMBOL;

    struct
    {
      Val *CAR;
      Val *CDR;
    } CONS;

    struct
    {
      char *MESSAGE;
    } ERROR;

    // used in eval step
    Builtin_Function BUILTIN;
    struct
    {
      Env *environment;
      Val *parameters;
      Val *body;
    } LAMBDA;

    struct
    {
      Env *environment;
      Val *parameters;
      Val *body;
    } MACRO;
  } as;

  Meta meta;
};

#define CAR(cons) ((cons)->as.CONS.CAR)
#define CDR(cons) ((cons)->as.CONS.CDR)

#define CAAR(cons) (CAR (CAR ((cons))))
#define CADR(cons) (CAR (CDR ((cons))))
#define CDAR(cons) (CDR (CAR ((cons))))
#define CDDR(cons) (CDR (CDR ((cons))))

#define IS_NULL(a) ((a) == NULL || (a)->type == VALUE_NIL)

Val *val_from_ast (AST *node);

Val *val_integer (long value);
Val *val_float (double value);
Val *val_string (const char *string);
Val *val_symbol (const char *symbol, Meta meta);
Val *val_cons (Val *car, Val *cdr);
Val *val_builtin (Builtin_Function builtin_function);

// special VALUE node builder, only for error messages
Val *val_error (const char *message);

Val *val_nil (void);
Val *val_t (void);

char *value_to_string (Val *node);
void value_print (Val *node);

#endif // VALUE_H_
