# tashya_aryan_2401MC03_OS_ASSIGNMENT09
- kernel/      modified xv6-public (x86) files + all_changes.diff (diff vs. original; every change is commented "Assignment 9")
- users/       pttest.c, faulttest.c, pagerepl.c
- screenshots/ terminal output captured from real QEMU runs, as .txt (add your own PNG screenshots here)
- REPORT.md    one-page report

Build/run: copy kernel/* over an xv6-public tree (replacing same-named files), copy users/* into it, then `make qemu-nox`
and run `pttest`, `faulttest`, `pagerepl`. The Makefile adds the three programs to UPROGS and two -Wno flags for newer GCC.
