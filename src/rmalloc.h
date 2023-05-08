/*
 * Copyright Redis Ltd. 2016 - present
 * Licensed under your choice of the Redis Source Available License 2.0 (RSALv2) or
 * the Server Side Public License v1 (SSPLv1).
 */

#ifndef __REDISEARCH_ALLOC__
#define __REDISEARCH_ALLOC__

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "redismodule.h"

#ifdef REDIS_MODULE_TARGET /* Set this when compiling your code as a module */

#ifdef MEMORY_DEBUG
#define rmalloc_MIN(x, y) (((x) < (y)) ? (x) : (y))
size_t getPointerAllocationSize(void *p);
extern size_t allocation_header_size;
extern uint64_t allocated;
extern uint64_t alloc_count;

static inline void *rm_malloc(size_t n) {
  size_t *ptr = (size_t *)RedisModule_Alloc(n + allocation_header_size);
  if (ptr) {
      allocated += n + allocation_header_size;
      alloc_count++;
      *ptr = n;
      return ptr + 1;
  }
  return NULL;
}
static inline void *rm_calloc(size_t nelem, size_t elemsz) {
  size_t *ptr = (size_t *)RedisModule_Calloc(1, (nelem * elemsz) + allocation_header_size);
  if (ptr) {
      allocated += (nelem * elemsz) + allocation_header_size;
      alloc_count++;
      *ptr = (nelem * elemsz);
      return ptr + 1;
  }
  return NULL;
}

static inline void rm_free(void *p) {
  if (!p)
      return;
  size_t *ptr = ((size_t *)p) - 1;
  allocated -= (ptr[0] + allocation_header_size);
  alloc_count--;
  RedisModule_Free(ptr);
}

static inline void *rm_realloc(void *p, size_t n) {
  if (n == 0) {
    rm_free(p);
    return NULL;
  }
  size_t oldSize = p ? getPointerAllocationSize(p) : 0;
  void *new_ptr = rm_malloc(n);
  if (new_ptr) {
      memcpy(new_ptr, p, rmalloc_MIN(oldSize, n));
      rm_free(p);
      return new_ptr;
  }
  return NULL;
}

static char *rm_strndup(const char *s, size_t n) {
  char *ret = (char *)rm_malloc(n + 1);

  if (ret) {
    ret[n] = '\0';
    memcpy(ret, s, n);
  }
  return ret;
}

static inline char *rm_strdup(const char *s) {
  return rm_strndup(s, strlen(s));
}

#else /* MEMORY_DEBUG */

static inline void *rm_malloc(size_t n) {
  return RedisModule_Alloc(n);
}

static inline void *rm_calloc(size_t nelem, size_t elemsz) {
  return RedisModule_Calloc(nelem, elemsz);
}

static inline void rm_free(void *p) {
  RedisModule_Free(p);
}

static inline void *rm_realloc(void *p, size_t n) {
  return RedisModule_Realloc(p, n);
}

static char *rm_strndup(const char *s, size_t n) {
  char *ret = (char *)rm_malloc(n + 1);

  if (ret) {
    ret[n] = '\0';
    memcpy(ret, s, n);
  }
  return ret;
}

static inline char *rm_strdup(const char *s) {
  return RedisModule_Strdup(s);
}

#endif /* MEMORY_DEBUG */

static int rm_vasprintf(char **__restrict __ptr, const char *__restrict __fmt, va_list __arg) {
  va_list args_copy;
  va_copy(args_copy, __arg);

  size_t needed = vsnprintf(NULL, 0, __fmt, __arg) + 1;
  *__ptr = (char *)rm_malloc(needed);

  int res = vsprintf(*__ptr, __fmt, args_copy);

  va_end(args_copy);

  return res;
}

static int rm_asprintf(char **__ptr, const char *__restrict __fmt, ...) {
  va_list ap;
  va_start(ap, __fmt);

  int res = rm_vasprintf(__ptr, __fmt, ap);

  va_end(ap);

  return res;
}
#endif
#ifndef REDIS_MODULE_TARGET
/* for non redis module targets */
#define rm_malloc malloc
#define rm_free free
#define rm_calloc calloc
#define rm_realloc realloc
#define rm_strdup strdup
#define rm_strndup strndup
#define rm_asprintf asprintf
#define rm_vasprintf vasprintf
#endif

#define rm_new(x) rm_malloc(sizeof(x))

#endif /* __RMUTIL_ALLOC__ */
