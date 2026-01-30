#ifndef CONSTROL_FLOW_H_
#define CONSTROL_FLOW_H_

#include "core/value.h"
#include "core/eval.h"

Val *builtin_eq (Val *environment, Val *arguments);
Val *builtin_if (Val *environment, Val *arguments);
Val *builtin_and (Val *environment, Val *arguments);
Val *builtin_or (Val *environment, Val *arguments);

#endif // CONSTROL_FLOW_H_
