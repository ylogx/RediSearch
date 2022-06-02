#include <stdio.h>
#include "heap.h"
#include "assert.h"
#include "redismodule.h"
#include "alloc.h"
REDISMODULE_INIT_SYMBOLS();

int cmp(void *a, void *b) {
  int *__a = (int *)a;
  int *__b = (int *)b;
  return *__a - *__b;
}

int main(int argc, char **argv) {
  RMUTil_InitAlloc();
  int myints[] = {10, 20, 30, 5, 15};
  Vector v = new Vector<int>(5);
  for (int i = 0; i < 5; i++) {
    v.Push(myints[i]);
  }

  Make_Heap(&v, 0, v.top, cmp);

  int n;
  v.Get(0, &n);
  assert(30 == n);

  Heap_Pop(&v, 0, v.top, cmp);
  v.top = 4;
  v.Get(0, &n);
  assert(20 == n);

  v.Push(99);
  Heap_Push(&v, 0, v.top, cmp);
  v.Get(0, &n);
  assert(99 == n);

  printf("PASS!\n");
  return 0;
}
