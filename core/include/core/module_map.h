#ifndef MODULE_MAP_H_
#define MODULE_MAP_H_

#include "core/environment.h"

void module_map_init ();
Value *module_map_get (const char *name);
void module_map_set (const char *name, Value *value);


#endif // MODULE_MAP_H_
