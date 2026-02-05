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

typedef struct Value Value;
typedef Value *(*Builtin_Function) (Environment *environment,
                                    Value *arguments);

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
      Value *CAR;
      Value *CDR;
    } CONS;

    struct
    {
      char *MESSAGE;
    } ERROR;

    // used in eval step
    Builtin_Function BUILTIN;
    struct
    {
      Environment *environment;
      Value *parameters;
      Value *body;
    } LAMBDA;

    struct
    {
      Environment *environment;
      Value *parameters;
      Value *body;
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

Value *val_from_ast (AST *node);

Value *val_integer (long value);
Value *val_float (double value);
Value *val_string (const char *string);
Value *val_symbol (const char *symbol, Meta meta);
Value *val_cons (Value *car, Value *cdr);
Value *val_builtin (Builtin_Function builtin_function);

// special VALUE node builder, only for error messages
Value *val_error (const char *message);

Value *val_nil (void);
Value *val_t (void);

char *value_to_string (Value *node);
void value_print (Value *node);

#endif // VALUE_H_
