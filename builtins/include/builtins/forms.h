#ifndef FORMS_H_
#define FORMS_H_

#include "core/value.h"
#include "core/eval.h"

Value *builtin_define (Environment *environment, Value *arguments);
Value *builtin_set (Environment *environment, Value *arguments);
Value *builtin_let (Environment *environment, Value *arguments);
Value *builtin_let_star (Environment *environment, Value *arguments);
Value *builtin_letrec (Environment *environment, Value *arguments);

Value *builtin_begin (Environment *environment, Value *arguments);
Value *builtin_eval (Environment *environment, Value *arguments);

Value *builtin_quote (Environment *environment, Value *arguments);
Value *builtin_quasiquote (Environment *environment, Value *arguments);

Value *builtin_lambda (Environment *environment, Value *arguments);
Value *builtin_macro (Environment *environment, Value *arguments);

#endif // FORMS_H_
