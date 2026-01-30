#ifndef FORMS_H_
#define FORMS_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_define (Val *environment, Val *arguments);
Val *builtin_set (Val *environment, Val *arguments);
Val *builtin_let (Val *environment, Val *arguments);
Val *builtin_let_star (Val *environment, Val *arguments);
Val *builtin_letrec (Val *environment, Val *arguments);

Val *builtin_begin (Val *environment, Val *arguments);
Val *builtin_eval (Val *environment, Val *arguments);

Val *builtin_quote (Val *environment, Val *arguments);
Val *builtin_quasiquote (Val *environment, Val *arguments);

Val *builtin_lambda (Val *environment, Val *arguments);
Val *builtin_macro (Val *environment, Val *arguments);

#endif // FORMS_H_
