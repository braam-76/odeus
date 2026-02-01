#ifndef FORMS_H_
#define FORMS_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_define (Env *environment, Val *arguments);
Val *builtin_set (Env *environment, Val *arguments);
Val *builtin_let (Env *environment, Val *arguments);
Val *builtin_let_star (Env *environment, Val *arguments);
Val *builtin_letrec (Env *environment, Val *arguments);

Val *builtin_begin (Env *environment, Val *arguments);
Val *builtin_eval (Env *environment, Val *arguments);

Val *builtin_quote (Env *environment, Val *arguments);
Val *builtin_quasiquote (Env *environment, Val *arguments);

Val *builtin_lambda (Env *environment, Val *arguments);
Val *builtin_macro (Env *environment, Val *arguments);

#endif // FORMS_H_
