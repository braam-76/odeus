#ifndef STRINGS_H_
#define STRINGS_H_

#include "core/ast.h"
#include "core/eval.h"

AST* builtin_concat(AST* environment, AST* arguments);
AST* builtin_string_length(AST* environment, AST* arguments);
AST* builtin_substring(AST* environment, AST* arguments);
AST* builtin_string_to_symbol(AST* environment, AST* arguments);
AST* builtin_symbol_to_string(AST* environment, AST* arguments);


#endif // STRINGS_H_
