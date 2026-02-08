#ifndef SYMBOL_MAP_H_
#define SYMBOL_MAP_H_

#include "core/value.h"

void symbol_map_init ();
Value *symbol_map_get (const char *name);
void symbol_map_set (const char *name, Value* value);

#endif // SYMBOL_MAP_H_
