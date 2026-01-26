#ifndef QUASIQUOTE_H_
#define QUASIQUOTE_H_

#include "core/ast.h"

AST *expand_quasiquote (AST *expr, int depth);

#endif // QUASIQUOTE_H_
