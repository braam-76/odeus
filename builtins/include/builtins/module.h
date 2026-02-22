#ifndef MODULE_H_
#define MODULE_H_

#include "core/eval.h"
#include "core/lexer.h"
#include "core/parser.h"
#include "core/ast.h"
#include "core/environment.h"
#include "core/value.h"

Value *builtin_load_module (Environment *environment, Value *arguments);
Value *builtin_get_from_module (Environment *environment, Value *arguments);
Value *builtin_reload_module (Environment *environment, Value *arguments);

#endif // MODULE_H_
