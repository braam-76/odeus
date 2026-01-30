#ifndef LIST_OPERATIONS_H_
#define LIST_OPERATIONS_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_cons (Val *environment, Val *arguments);
Val *builtin_list (Val *environment, Val *arguments);

Val *builtin_car (Val *environment, Val *arguments);
Val *builtin_cdr (Val *environment, Val *arguments);
Val *builtin_set_car (Val *environment, Val *arguments);
Val *builtin_set_cdr (Val *environment, Val *arguments);
Val *builtin_length (Val *environment, Val *arguments);
Val *builtin_reverse (Val *environment, Val *arguments);
Val *builtin_filter (Val *environment, Val *arguments);
Val *builtin_apply (Val *environment, Val *arguments);

#endif // LIST_OPERATIONS_H_
