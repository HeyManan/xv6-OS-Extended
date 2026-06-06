# xv6-OS-Extended

Kernel-level extensions to **xv6**, the MIT teaching operating system, implemented in C and x86 assembly. The work spans four projects covering custom **system calls**, **shell commands**, **CPU schedulers**, **virtual memory**, and **file systems** — touching the kernel from the syscall path down to page-fault handling and on-disk inode structures.

> Coursework for *COP6611 — Operating Systems*, University of South Florida (Fall 2025).

---

## Repository structure

Each project lives on its own branch so the work can be browsed independently.

| Branch | Project | Focus |
|--------|---------|-------|
| `main` | — | This overview |
| `Project1` | System Calls & Shell Commands | New syscall, `ls`/`sleep` commands |
| `Project2` | System Calls & Schedulers | `sort`/`grep` flags, SJF + Priority RR schedulers |
| `Project3` | Virtual Memory | Lazy & locality-aware page allocation |
| `Project4` | File Systems | `lseek`, symbolic links, large files |

---

## Project 1 — System Calls & Shell Commands

- Added a custom **`hello` system call** (the 22nd in xv6), wiring it through the complete syscall path: `syscall.h`, `syscall.c`, `sysproc.c`, `user.h`, and the `usys.S` trap stub.
- Extended the **`ls`** command to hide dotfiles, append `/` to directory names, and added a **`-s` flag** that sorts entries by file size using a heap sort over a custom `FileInfo` struct.
- Implemented the **`sleep`** user command with argument validation.

## Project 2 — System Calls & Schedulers

- Implemented **`sort`** (selection sort) with `-r`, `-u`, and `-n` flags and pipe support, and extended **`grep`** with `-i`, `-n`, and `-v` flags by modifying its `matchhere`/`matchstar` matching routines.
- Added a **`ticks_running`** system call that reports per-process scheduled ticks (returning `0` for an unscheduled process and `-1` for a nonexistent one), with a `ticktest` verification program.
- Built two pluggable **CPU schedulers** — **Shortest-Job-First** and a **multi-level Priority Round-Robin** (high/medium/low, round-robin within a level) — selectable via a Makefile flag, each with a fork-based test program (`sjftest`, `prtest`) that verifies execution order.

## Project 3 — Virtual Memory

- Converted `sbrk()` to **lazy allocation** by removing the eager `growproc()` call, then handled the resulting page faults (**trap 14 / `T_PGFLT`**) in a custom trap handler that reads the faulting address via `rcr2()`, validates it against the process high-water mark (`myproc()->sz`), and maps a fresh page with `kalloc()` + `mappages()`.
- Added a **locality-aware allocator** (toggled by a `Makefile` build flag) that maps multiple pages per fault.
- **Result:** on a custom test walking 10 pages sequentially, the locality-aware allocator cut page faults from **10 → 4 (~60%)** versus lazy allocation — while the analysis also documents its trade-off of over-allocating on sparse access patterns.

## Project 4 — File Systems

- Added an **`lseek`** system call (in `sysfile.c`) and modified `filewrite` to handle **file holes** by zero-filling the gap between the old file size and a seek offset beyond it.
- Implemented **symbolic links**: a new `T_SYMLINK` inode type, an `O_NOFOLLOW` open flag, a `symlink` system call, and resolution logic in `sys_open` with **cycle detection** (capped at 10 hops) to avoid infinite loops.
- Added support for **large files (16MB+)** by introducing **doubly-indirect blocks** — reducing `NDIRECT`, updating `MAXFILE`/`NDINDIRECT`, and extending `bmap` to navigate the double-indirection tree, with `memset` zeroing of new pointer blocks to fix an uninitialized-data bug. Verified by successfully writing ~32,900 blocks (~16.8 MB).

---

## Build & run

xv6 builds and runs in QEMU. Pick a project branch, then build:

```bash
git clone https://github.com/HeyManan/xv6-OS-Extended.git
cd xv6-OS-Extended
git checkout Project3          # or Project1 / Project2 / Project4
make clean && make qemu-nox
```

(Exit the emulator with `Ctrl-a` then `x`.)

Project 3's allocator can be selected at build time, e.g.:

```bash
make clean && make ALLOCATOR=LOCALITY qemu-nox   # or ALLOCATOR=LAZY
```

> Build flags (scheduler/allocator selection) follow each branch's `Makefile` — check the project README on the branch for exact options.

## Environment

C · x86 assembly · QEMU · GDB · GNU Make · Linux/Unix · Git

## Acknowledgements

Completed under the instruction of Dr. John Templeton (COP6611, USF), with thanks to classmates for design discussions. Detailed per-project notes are included in each branch's README.
