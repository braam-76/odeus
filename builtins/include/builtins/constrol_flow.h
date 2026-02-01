#ifndef CONSTROL_FLOW_H_
#define CONSTROL_FLOW_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_eq (Env *environment, Val *arguments);
Val *builtin_if (Env *environment, Val *arguments);
Val *builtin_and (Env *environment, Val *arguments);
Val *builtin_or (Env *environment, Val *arguments);

#endif // CONSTROL_FLOW_H_
