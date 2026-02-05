#include "core/intern_string.h"

#define MAX_STRINGS 8192
static const char *string_pool[MAX_STRINGS];
static int pool_count = 0;

const char *
intern_string (const char *s)
{
  for (int i = 0; i < pool_count; i++)
    if (strcmp (string_pool[i], s) == 0)
      return string_pool[i];

  char *news = strdup (s);
  string_pool[pool_count++] = news;
  return news;
}
