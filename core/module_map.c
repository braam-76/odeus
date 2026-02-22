#include "core/module_map.h"
#include "core/environment.h"
#include "core/value.h"
#include <gc/gc.h>
#include <stdint.h>
#include <string.h>

#define INITIAL_CAPACITY 2048
#define LOAD_FACTOR 0.8

typedef struct
{
  const char *key;
  Value *value;
} Entry;

static Entry *table = NULL;
static size_t capacity = 1;
static size_t size = 0;

static void
resize (size_t new_capacity)
{
  Entry *old = table;
  size_t old_capacity = capacity;

  table = GC_malloc (new_capacity * sizeof (Entry));
  memset (table, 0, new_capacity * sizeof (Entry));
  capacity = new_capacity;
  size = 0;

  if (!old)
    return;

  for (size_t i = 0; i < old_capacity; i++)
    if (old[i].key)
      module_map_set (old[i].key, old[i].value);
}

static uint32_t
hash (const char *s)
{
  uint32_t h = 2166136261u;
  for (; *s; s++)
    {
      h ^= (unsigned char)*s;
      h *= 16777619u;
    }
  return h;
}

void
module_map_init ()
{
  resize (INITIAL_CAPACITY);
}

Value *
module_map_get (const char *name)
{
  if (size == 0)
    return NULL;

  uint32_t h = hash (name);
  size_t i = h % capacity;

  while (true)
    {
      if (!table[i].key)
        return NULL;
      if (strcmp (table[i].key, name) == 0)
        return table[i].value;

      i = (i + 1) % capacity;
    }
}

void
module_map_set (const char *name, Value *value)
{
  if ((double)(size + 1) / capacity > LOAD_FACTOR)
    resize (capacity * 2);

  uint32_t h = hash (name);
  size_t i = h % capacity;

  while (true)
    {
      if (!table[i].key)
        {
          table[i].key = name;
          table[i].value = value;
          size++;
          return;
        }

      if (strcmp (table[i].key, name) == 0)
        {
          table[i].value = value;
          return;
        }

      i = (i + 1) % capacity;
    }
}
