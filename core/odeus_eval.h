#ifndef ODEUS_EVAL_H_
#define ODEUS_EVAL_H_

#include "odeus_parser.h"

void environment_set (AST_Node *env, AST_Node *symbol, AST_Node *value);
AST_Node *environment_get (AST_Node *env, AST_Node *symbol);
AST_Node *evaluate_expression (AST_Node *environment, AST_Node *expression);

#endif // ODEUS_EVAL_H_
