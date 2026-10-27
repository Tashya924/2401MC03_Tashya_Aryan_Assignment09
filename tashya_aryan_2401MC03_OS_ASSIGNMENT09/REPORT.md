# OS Assignment 9 – Report (Tashya Aryan, 2401MC03)

**Q1 – vmprint().** Added `sys_vmprint` -> `vmprint(pgdir)` in vm.c. It walks the page directory entries below KERNBASE, scans each present second-level table, and prints VA, PA and P/W/U for every present PTE, then the number of valid user pages and page-table pages (second-level tables + the page directory). `pttest` output: 3 pages at start; still 3 right after `sbrk(4*4096)` (lazy allocation maps nothing yet); 7 after touching each new page, i.e. exactly 4 new pages. Growing the logical size (`p->sz`) is different from creating PTEs: mappings appear only when a page is first used.

**Q2 – Lazy allocation.** `sys_sbrk(n>0)` only does `sz += n`. `deallocuvm` and `copyuvm` skip absent PDEs/PTEs instead of panicking (the child keeps the parent's `sz` and gets holes lazily; also made `uva2ka` null-safe). `trap.c` handles `T_PGFLT` in user mode: if `rcr2() < sz` and the page is not present, it does `kalloc`, `memset`, and `mappages(PTE_W|PTE_U)` (frees the page if mapping fails), increments `p->nfaults`, and resumes. Otherwise it prints pid and address and kills only that process. `getfaults()` returns `nfaults` (zeroed at process creation; a forked child starts at 0). `faulttest`: `sbrk(32*4096)`, five writes to five different pages (plus a second write per page, which causes no fault) -> **5** faults, PASS. A child touching 0x24000 (above sz) was killed; the parent continued.

**Q3 – FIFO vs LRU.** `pagerepl.c` simulates both algorithms (FIFO order is not changed by hits; LRU refreshes on hits) and prints frames plus HIT/FAULT for each reference. For `7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1` with 3 frames: FIFO **15** faults, LRU **12** faults (match the assignment).

| Algorithm | Frames | Faults | Hits | Hit ratio |
|---|---:|---:|---:|---:|
| FIFO | 3 | 15 | 5 | 0.250 |
| FIFO | 4 | 10 | 10 | 0.500 |
| FIFO | 5 | 9 | 11 | 0.550 |
| LRU | 3 | 12 | 8 | 0.400 |
| LRU | 4 | 8 | 12 | 0.600 |
| LRU | 5 | 7 | 13 | 0.650 |

LRU is better at every frame size. **Note:** Q1 and Q2 conflict (eager vs lazy sbrk); the final kernel is lazy and `pttest` touches the new pages to show the four new mappings.
