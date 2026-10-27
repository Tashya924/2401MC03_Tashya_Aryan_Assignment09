// Assignment 9 Q2: lazy allocation test.
#include "types.h"
#include "stat.h"
#include "user.h"

#define PG 4096

int
main(void)
{
  char *base;
  int i, pid, before;
  int pages[5] = {1, 7, 12, 20, 31};   // five distinct pages out of 32

  before = getfaults();
  base = sbrk(32 * PG);   // logical growth only; no physical pages yet
  printf(1, "sbrk(32*4096) returned 0x%x, faults so far: %d\n",
         (uint)base, getfaults() - before);

  for(i = 0; i < 5; i++){
    base[pages[i] * PG] = 'x';        // first touch: 1 fault
    base[pages[i] * PG + 100] = 'y';  // same page: no new fault
  }
  printf(1, "lazy-allocation faults handled: %d (expected 5)\n",
         getfaults() - before);
  if(getfaults() - before == 5)
    printf(1, "PASS\n");
  else
    printf(1, "FAIL\n");

  // Invalid access (above sz) in a child: only the child must die.
  pid = fork();
  if(pid == 0){
    volatile char *bad = base + 33 * PG;   // beyond sbrk'd region
    *bad = 1;
    printf(1, "child: SHOULD NOT REACH HERE\n");
    exit();
  }
  wait();
  printf(1, "parent: child was killed, parent still running\n");
  exit();
}
