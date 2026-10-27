# Operating Systems — Assignment 9

**Student:** Tashya Aryan  
**Roll Number:** 2401MC03  
**Course:** Operating Systems  
**Repository:** [2401MC03_Tashya_Aryan_Assignment09](https://github.com/Tashya924/2401MC03_Tashya_Aryan_Assignment09)

## Overview

This assignment explores virtual memory management, lazy memory allocation, page-fault handling, and page-replacement algorithms using xv6.

It consists of three questions:

- **Q1: Page Table Printing** — Implementing `vmprint()` to inspect the virtual memory mappings of a process.
- **Q2: Lazy Allocation** — Deferring physical memory allocation until a page is first accessed and handling page faults.
- **Q3: Page Replacement** — Simulating and comparing FIFO and LRU page-replacement algorithms.

## Repository Structure

```text
2401MC03_Tashya_Aryan_Assignment09/
├── kernel/
│   ├── all_changes.diff
│   ├── defs.h
│   ├── proc.c
│   ├── proc.h
│   ├── syscall.c
│   ├── syscall.h
│   ├── sysproc.c
│   ├── trap.c
│   ├── user.h
│   ├── usys.S
│   ├── vm.c
│   └── Makefile
├── users/
│   ├── pttest.c
│   ├── faulttest.c
│   └── pagerepl.c
├── screenshots/
│   ├── pttest.png
│   ├── faulttest.png
│   └── pagerepl.png
├── REPORT.md
└── README.md
```

## Question 1 — Printing the Page Table

### Objective

Implement a `vmprint()` system call that displays the page-table mappings of the calling process.

### Implementation

- Traverse the process's page table and inspect valid user-page mappings.
- Print virtual addresses, physical addresses, and the permission flags: Present (P), Writable (W), and User-accessible (U).
- Count valid user pages and page-table pages.
- Ignore the kernel portion of the address space as specified in the assignment.

### Test Program: `users/pttest.c`

The test program:

1. Prints the initial page table.
2. Calls `sbrk(4 * 4096)` to request four pages.
3. Prints the page table before accessing the new region.
4. Writes one byte to each of the four pages.
5. Prints the page table again to observe the new mappings.

**Expected observation:** Four additional valid user-page mappings should appear after the four pages are first accessed when lazy allocation is enabled.

## Question 2 — Lazy Allocation and Page Faults

### Objective

Modify xv6 to allocate physical memory only when a process first accesses a valid, previously unmapped page.

### Implementation

- Modify `sys_sbrk()` to increase the process's logical memory size without immediately allocating physical pages.
- Handle page faults in the kernel by checking the faulting address.
- Allocate and zero a physical page for a valid lazy-allocation fault, then map it into the process's address space.
- Maintain a per-process page-fault counter.
- Implement the `getfaults()` system call to retrieve the fault count.
- Update memory deallocation and address-space copying logic to handle pages that have not yet been allocated.
- Terminate a process that accesses an address outside its permitted memory region.

### Test Program: `users/faulttest.c`

The test program:

1. Requests 32 pages using `sbrk(32 * 4096)`.
2. Writes to five distinct pages.
3. Writes again to the same pages to verify that they are already mapped.
4. Checks the number of newly handled lazy-allocation faults.
5. Forks a child process and deliberately accesses an address beyond the allocated region.

**Expected results:**

- Exactly five new lazy-allocation faults are handled.
- The test prints `PASS` if the fault count matches the expected value.
- The invalid access terminates the child while the parent continues running.

## Question 3 — FIFO and LRU Page Replacement

### Objective

Implement a page-replacement simulation that compares First In, First Out (FIFO) and Least Recently Used (LRU).

### Test Program: `users/pagerepl.c`

The program:

- Accepts a frame count and an optional page-reference string.
- Uses a default reference string when no custom string is supplied.
- Prints the frame contents and hit/fault status for every reference.
- Calculates the page-fault count, hit count, and hit ratio.
- Compares both algorithms using three, four, and five frames.

Default reference string:

```text
7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
```

### Expected Results for Three Frames

| Algorithm | Frames | Expected Page Faults |
|---|---:|---:|
| FIFO | 3 | 15 |
| LRU | 3 | 12 |

The program also produces a comparison table for three, four, and five frames.

The hit ratio is calculated as:

\[
\text{Hit Ratio}=\frac{\text{Number of Hits}}{\text{Total References}}
\]

## Build and Run

Use a compatible xv6 source tree containing the modified kernel files and user programs.

1. Copy the modified kernel files into the corresponding locations in the xv6 source tree.
2. Copy the three test programs into its user-program directory.
3. Ensure the Makefile includes `pttest`, `faulttest`, and `pagerepl` in the user-program build list.
4. Build and launch xv6:

```bash
make qemu
```

5. Run the tests from the xv6 shell:

```text
$ pttest
$ faulttest
$ pagerepl
```

To run the page-replacement simulation with a custom reference string:

```text
$ pagerepl 3 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
```

**Note:** Build commands and kernel interfaces may differ depending on the xv6 version used. Ensure the implementation matches the target architecture and source tree.

## Screenshots

The `screenshots/` directory is intended to contain terminal screenshots demonstrating the results of all three test programs.

- `pttest.png` — Page-table output before and after accessing the four new pages.
- `faulttest.png` — Lazy-allocation fault count and child-process termination test.
- `pagerepl.png` — FIFO/LRU execution traces and comparison table.

For final submission, use screenshots captured from actual QEMU runs.

## Files and Documentation

- `kernel/all_changes.diff` — Summary of kernel modifications relative to the original source.
- `REPORT.md` — Short report describing the implementation and observed results.
- `users/` — User-space test programs for the three assignment questions.
- `screenshots/` — Terminal evidence for the test runs.

## Conclusion

This assignment demonstrates how page tables represent virtual memory mappings, how lazy allocation reduces unnecessary physical-memory allocation, how page faults can be handled safely by the kernel, and how FIFO and LRU make page-replacement decisions based on different policies.

The three test programs provide a way to inspect memory mappings, validate lazy-allocation behavior, and compare page-replacement algorithms.
