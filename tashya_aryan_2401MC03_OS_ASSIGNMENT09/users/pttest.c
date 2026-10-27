// Assignment 9 Q1: print the page table before and after sbrk(4*4096).
// NOTE: the kernel now uses lazy allocation (Q2), so sbrk() alone only
// grows the logical size (p->sz).  Mappings appear when pages are first
// touched, so we write one byte to each of the four new pages.  This
// shows exactly four new valid user pages.
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int i;

  printf(1, "=== vmprint at start ===\n");
  vmprint();

  p = sbrk(4 * 4096);
  printf(1, "\n=== after sbrk(4*4096), nothing touched (lazy: no new mappings) ===\n");
  vmprint();

  for(i = 0; i < 4; i++)
    p[i * 4096] = 1;   // one touch per page -> one page fault each
  printf(1, "\n=== after touching each of the 4 new pages ===\n");
  vmprint();
  exit();
}
