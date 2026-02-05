#ifndef LIST_OPERATIONS_H_
#define LIST_OPERATIONS_H_

#include "core/value.h"
#include "core/eval.h"

Value *builtin_cons (Environment *environment, Value *arguments);
Value *builtin_list (Environment *environment, Value *arguments);

Value *builtin_car (Environment *environment, Value *arguments);
Value *builtin_cdr (Environment *environment, Value *arguments);
Value *builtin_set_car (Environment *environment, Value *arguments);
Value *builtin_set_cdr (Environment *environment, Value *arguments);
Value *builtin_length (Environment *environment, Value *arguments);
Value *builtin_reverse (Environment *environment, Value *arguments);
Value *builtin_filter (Environment *environment, Value *arguments);
Value *builtin_apply (Environment *environment, Value *arguments);

#endif // LIST_OPERATIONS_H_
