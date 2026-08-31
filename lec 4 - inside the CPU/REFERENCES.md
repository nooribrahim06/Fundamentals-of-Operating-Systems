# Sources, Slide Map & Clarifications

> The **full course explanation now lives in `README.md`**. This file is only the source/slide map and correction log.

## Course Basis

This repo is grounded in the supplied *Fundamentals of Operating Systems* material, **slides 179–217**, plus the four supplied lecture transcripts covering:

1. CPU components / caches / NUMA / RISC-CISC.
2. Instruction lifecycle.
3. Pipelining / parallelism / Hyper-Threading / SIMD.
4. CPU wait times / `top` / PostgreSQL demo.

## Slide Map

| Slides | Topic |
|---|---|
| 179–180 | Inside the CPU / section intro |
| 181–190 | CPU components, multicore layout, NUMA/DSM, ALU, CU, registers, MMU/TLB, caches |
| 191–194 | RISC/CISC, example, clock speed, macOS commands |
| 195 | components summary |
| 196–207 | instruction lifecycle walkthrough + cache line + inlining teaser |
| 208–214 | pipelining, parallelism, Hyper-Threading, SIMD |
| 215–217 | CPU wait-times introduction/demo |

Only the slide images embedded in `README.md` are retained under `assets/slides/` for revision.

## Extra Supplied References Used in the Lecture/Chat

- **ARM Address Space ID** reference — supports ASID-tagged TLB translations reducing context-switch flush overhead.
- **Optimized Linux `memchr()`** article — word-wide comparison optimization, reported large speedup on long searches.
- **A War Story About a Python, a Fork, a CoW, and a Bug** — CPython refcount writes, Copy-on-Write production-memory story, immortal objects.

## Clarifications Added So You Don't Memorize Oversimplifications

These are **clarifications beyond the literal slide bullets**:

1. **Cache sizes/latencies vary.** Memorize hierarchy, not exact ns/KB/MB values.
2. **Modern CPU control logic is distributed/complex.** “CU” is a teaching abstraction.
3. **PC increment is ISA-dependent.** AArch64 is normally 4-byte fixed instructions; x86-64 is variable-length.
4. **RISC ≠ always one cycle; CISC ≠ always many cycles.** Modern microarchitectures are far more complex.
5. **DMA does not universally bypass translation.** IOMMUs can translate/protect device DMA accesses.
6. **NUMA is not a literal shared-bus-lock story.** Modern machines use coherent point-to-point interconnects; the durable concept is local vs remote memory cost.
7. **SMT sharing details vary.** Do not assume exact cache/TLB partitioning from the teaching drawing.
8. **Linux iowait is subtle.** It is idle-time accounting in the presence of outstanding block I/O, not “a CPU process is physically frozen”.
9. **Spectre is a microarchitectural side-channel story.** Wrong-path architectural state is rolled back, but cache/timing traces can remain.
10. **MySQL 8 code-size/iTLB comment is a theory/example from the transcript**, not a settled universal cause.

## Repo Philosophy

- No article-style walls of text.
- Every concept has a short definition, diagram, contrast, or table.
- The README is the map; each note is a focused revision unit.
- The final bridge file intentionally prepares the next course section on processes and threads.
