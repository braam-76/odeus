#ifndef QUASIQUOTE_H_
#define QUASIQUOTE_H_

#include "core/value.h"

Val *expand_quasiquote (Val *expr, int depth);

#endif // QUASIQUOTE_H_
