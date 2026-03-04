#include "builtins/module.h"

#include "builtins/stdio.h"
#include "core/environment.h"
#include "core/eval.h"
#include "core/value.h"
#include <gc/gc.h>
#include <string.h>

Value *
builtin_import (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("import: expects exactly 1 arguments");

  Value *module_name = CAR (arguments);
  ERROR_OUT (module_name);
  if (module_name->type != VALUE_STRING && module_name->type != VALUE_SYMBOL)
    return val_error ("import: first argument is neither a string or symbol");

  if (module_name->type == VALUE_SYMBOL)
    return val_error ("import: for now import does not support symbol as argument, "
                      "as it meant to be used for std-only modules."
                      "And there is no established std modules yet, sorry :(");

  if (module_name->type == VALUE_STRING)
    {
      char *module_name_cstr = strdup (module_name->as.STRING);

      Value *load_file_args
          = val_cons (val_string (module_name_cstr), val_nil ());

      Environment *module_environment = env_init (environment);

      Value *err = builtin_load_file (module_environment, load_file_args);
      free (module_name_cstr);

      ERROR_OUT (err);
      return val_module (module_name->as.SYMBOL, module_environment);
    }

  return val_error("import: something went wrong, and developer did not handle it");
}

Value *
builtin_get_from_module (Environment *environment, Value *arguments)
{
  (void)environment;

  if (arguments_length (arguments) != 2)
    return val_error ("get-from-module: expects exactly 2 arguments");

  Value *module = evaluate_expression(environment, CAR (arguments));
  ERROR_OUT (module);
  if (module->type != VALUE_MODULE)
    return val_error ("get-from-module: first argument is not module");

  Value *symbol_name = CADR (arguments);

  ERROR_OUT (symbol_name);
  if (symbol_name->type != VALUE_SYMBOL)
    return val_error ("get-from-symbol: second argument is not symbol");

  Value *result = env_get (module->as.MODULE.environment, symbol_name);

  return result;
}
