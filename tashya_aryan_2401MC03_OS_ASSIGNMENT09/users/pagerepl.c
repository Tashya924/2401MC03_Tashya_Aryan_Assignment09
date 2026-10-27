// Assignment 9 Q3: FIFO vs LRU page-replacement simulation.
// Usage: pagerepl                      (default string, 3/4/5 frames + table)
//        pagerepl <frames> p1 p2 ...   (detail run for a given string)
#include "types.h"
#include "stat.h"
#include "user.h"

#define MAXF 16
#define MAXR 64

static int refs[MAXR];
static int nref;

static void
printframes(int *fr, int nf)
{
  int i;
  printf(1, "[");
  for(i = 0; i < nf; i++){
    if(fr[i] < 0) printf(1, " -");
    else printf(1, " %d", fr[i]);
  }
  printf(1, " ]");
}

// Runs one algorithm (lru=0 -> FIFO, lru=1 -> LRU); returns fault count.
static int
simulate(int nf, int lru, int verbose)
{
  int fr[MAXF];
  int stamp[MAXF];   // FIFO: time loaded (never updated on hit); LRU: time last used
  int i, r, k, hit, victim, faults = 0;

  for(i = 0; i < nf; i++){ fr[i] = -1; stamp[i] = 0; }
  for(r = 0; r < nref; r++){
    int pg = refs[r];
    hit = -1;
    for(i = 0; i < nf; i++)
      if(fr[i] == pg){ hit = i; break; }
    if(hit >= 0){
      if(lru) stamp[hit] = r + 1;   // only LRU refreshes on a hit
    } else {
      faults++;
      victim = -1;
      for(i = 0; i < nf; i++)
        if(fr[i] < 0){ victim = i; break; }   // free frame first
      if(victim < 0){
        victim = 0;
        for(k = 1; k < nf; k++)       // smallest stamp = oldest/least recent
          if(stamp[k] < stamp[victim]) victim = k;
      }
      fr[victim] = pg;
      stamp[victim] = r + 1;
    }
    if(verbose){
      printf(1, "ref %d: ", pg);
      printframes(fr, nf);
      printf(1, " %s\n", hit >= 0 ? "HIT" : "FAULT");
    }
  }
  return faults;
}

// Print hits/total as 0.xxx using integer arithmetic (no floats in xv6).
static void
printratio(int hits, int total)
{
  int m = (hits * 1000 + total / 2) / total;
  printf(1, "%d.%d%d%d", m / 1000, (m / 100) % 10, (m / 10) % 10, m % 10);
}

int
main(int argc, char *argv[])
{
  static char *def = "7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1";
  int nf = 3, i, a, f, hits;

  if(argc >= 3){
    nf = atoi(argv[1]);
    for(i = 2; i < argc && nref < MAXR; i++)
      refs[nref++] = atoi(argv[i]);
  } else {
    char *p = def;
    while(*p){
      if(*p == ' ') p++;
      else { refs[nref++] = atoi(p); while(*p && *p != ' ') p++; }
    }
  }
  if(nf < 1 || nf > MAXF || nref == 0){
    printf(2, "usage: pagerepl [frames ref1 ref2 ...]\n");
    exit();
  }

  printf(1, "=== FIFO, %d frames ===\n", nf);
  f = simulate(nf, 0, 1);
  printf(1, "FIFO page faults: %d\n\n", f);
  printf(1, "=== LRU, %d frames ===\n", nf);
  f = simulate(nf, 1, 1);
  printf(1, "LRU page faults: %d\n\n", f);

  printf(1, "Algorithm Frames Faults Hits HitRatio (refs=%d)\n", nref);
  for(a = 0; a < 2; a++){
    int n;
    for(n = 3; n <= 5; n++){
      f = simulate(n, a, 0);
      hits = nref - f;
      printf(1, "%s       %d      %d     %d    ", a ? "LRU " : "FIFO", n, f, hits);
      printratio(hits, nref);
      printf(1, "\n");
    }
  }
  exit();
}
