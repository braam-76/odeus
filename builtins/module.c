#include "builtins/module.h"

#include "builtins/stdio.h"
#include "core/environment.h"
#include "core/eval.h"
#include "core/module_map.h"
#include "core/value.h"
#include <gc/gc.h>
#include <string.h>

Value *
builtin_load_module (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("load-module: expects exactly 1 arguments");

  Value *module_name = CAR (arguments);
  ERROR_OUT (module_name);
  if (module_name->type != VALUE_SYMBOL)
    return val_error ("load-module: first argument is not a symbol");

  char *module_name_cstr = strdup (module_name->as.SYMBOL);
  strcat (module_name_cstr, ".ode");

  Value *load_file_args = val_cons (val_string (module_name_cstr), val_nil ());

  Environment *module_environment = env_init (environment);

  Value *err = builtin_load_file (module_environment, load_file_args);
  free (module_name_cstr);

  ERROR_OUT (err);
  return val_module (module_name->as.SYMBOL, module_environment);
}

Value *
builtin_get_from_module (Environment *environment, Value *arguments)
{
  (void)environment;

  if (arguments_length (arguments) != 2)
    return val_error ("get-from-module: expects exactly 2 arguments");

  Value *module_name = CAR (arguments);
  ERROR_OUT (module_name);
  if (module_name->type != VALUE_SYMBOL)
    return val_error (
        "get-from-module: first argument (module) is not symbol");

  Value *symbol_name = CADR (arguments);
  ERROR_OUT (symbol_name);
  if (symbol_name->type != VALUE_SYMBOL)
    return val_error (
        "get-from-symbol: first argument (symbol) is not symbol");

  Value *module = module_map_get (module_name->as.SYMBOL);
  ERROR_OUT (module);
  if (!module || module->type != VALUE_MODULE)
    return val_error ("get-from-symbol: Module corrupted");

  Value *result = env_get (module->as.MODULE.environment, symbol_name);

  return result;
}

Value *
builtin_reload_module (Environment *environment, Value *arguments)
{
  if (arguments_length (arguments) != 1)
    return val_error ("reload-module: expects exactly one argument");

  Value *module_name = CAR (arguments);
  if (module_name->type != VALUE_SYMBOL)
    return val_error ("reload-module: argument is not symbol");

  char *module_name_cstr = module_name->as.SYMBOL;

  Value *module = module_map_get (module_name_cstr);
  if (!module)
    return val_error ("Module %s not found", module_name_cstr);

  return builtin_load_module (environment, arguments);
}
