#ifndef LIST_OPERATIONS_H_
#define LIST_OPERATIONS_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_cons (Env *environment, Val *arguments);
Val *builtin_list (Env *environment, Val *arguments);

Val *builtin_car (Env *environment, Val *arguments);
Val *builtin_cdr (Env *environment, Val *arguments);
Val *builtin_set_car (Env *environment, Val *arguments);
Val *builtin_set_cdr (Env *environment, Val *arguments);
Val *builtin_length (Env *environment, Val *arguments);
Val *builtin_reverse (Env *environment, Val *arguments);
Val *builtin_filter (Env *environment, Val *arguments);
Val *builtin_apply (Env *environment, Val *arguments);

#endif // LIST_OPERATIONS_H_
