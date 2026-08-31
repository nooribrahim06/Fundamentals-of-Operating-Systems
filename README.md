# Operating Systems Fundamentals

A backend-focused roadmap for understanding what an operating system actually does—from CPU execution and virtual memory to processes, storage, sockets, virtualization, and containers.

**One question drives the whole repository:** what really happens after an application asks the computer to do work?

Each lecture builds on the previous one. Every folder has a detailed `README.md`, selected diagrams, and—where useful—small C demos, labs, quizzes, cheat sheets, or references.

## Credit

The concepts in this repository were learned from Hussein Nasser's Udemy course, [Fundamentals of Operating Systems](https://www.udemy.com/course/fundamentals-of-operating-systems/).

The explanations, revision structure, generated diagrams, and demo reconstructions are personal study notes. Only the course-slide excerpts that are directly used in a lecture README are retained for educational context; the complete slide deck is not included.

## Lectures

| # | Lecture | Covers |
|---:|---|---|
| 01 | [Why Do We Need an Operating System?](lec%201%20-%20system%20arch/README.md) | OS abstractions, scarce-resource scheduling, kernel vs distro, CPU, memory, storage, filesystems, networking, drivers, interrupts, and system calls |
| 02 | [The Anatomy of a Process](lec%202%20-%20anatomy%20of%20a%20process/README.md) | Program vs process, compilation and linking, CPU execution, text/data/heap/stack, function calls, allocation, and Linux process mappings |
| 03 | [Memory Management](lec%203%20-%20memory%20management/Memory-Management/README.md) | SRAM/DRAM/DDR, locality, fragmentation, paging, virtual memory, page tables, TLBs, swap, DMA, and IOMMUs |
| 04 | [Inside the CPU](lec%204%20-%20inside%20the%20CPU/README.md) | Cores, registers, caches, coherence, NUMA, MMU/TLB, instruction execution, pipelining, SMT, SIMD, and workload behavior |
| 05 | [Process Management](lec%205%20-%20process%20management/README.md) | Processes and threads, `fork()`, Copy-on-Write, context switching, scheduling, concurrency, races, mutexes, and semaphores |
| 06 | [Storage Management](lec%206%20-%20storage%20management/README.md) | HDDs and SSDs, LBA, NAND behavior, filesystems, page cache, durability, partitions, mounting, and the full `read()` path |
| 07 | [Socket Management](lec%207%20-%20socket%20management/README.md) | Listening and connected sockets, kernel queues and buffers, blocking I/O, `epoll`, `io_uring`, libuv, and backend server patterns |
| 08 | [More OS Concepts](lec%208%20-%20more%20os%20concepts/README.md) | Compilers, linkers, runtimes, JIT and GC, user/kernel mode, virtualization, containers, namespaces, cgroups, and OverlayFS |

## How to Use This Repository

Start with Lecture 01 and follow the sequence. The main READMEs are written as connected revision guides, while supporting files let you go deeper:

- `examples/` and `code/` contain small demonstrations you can compile and inspect.
- `labs/` contain optional hands-on exercises.
- `QUIZ.md`, `CHEATSHEET.md`, and `QUICK-RECALL.md` provide active recall and fast revision where available.
- `REFERENCES.md` and `references/` collect sources and deeper reading.

The goal is not to memorize isolated definitions. It is to build one mental model connecting an application, the kernel, and the hardware underneath it.
