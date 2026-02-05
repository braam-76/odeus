#ifndef CONSTROL_FLOW_H_
#define CONSTROL_FLOW_H_

#include "core/value.h"
#include "core/eval.h"

Value *builtin_eq (Environment *environment, Value *arguments);
Value *builtin_if (Environment *environment, Value *arguments);
Value *builtin_and (Environment *environment, Value *arguments);
Value *builtin_or (Environment *environment, Value *arguments);

#endif // CONSTROL_FLOW_H_
