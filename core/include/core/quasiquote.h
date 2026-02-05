#ifndef QUASIQUOTE_H_
#define QUASIQUOTE_H_

#include "core/value.h"

Value *expand_quasiquote (Value *expr, int depth);

#endif // QUASIQUOTE_H_
