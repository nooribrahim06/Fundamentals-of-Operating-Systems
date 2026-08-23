# CPU Section — 2-Minute Cheatsheet

## Components

```text
ALU      = arithmetic/logic execution
Control  = fetch/decode/coordinate
Registers= tiniest fastest execution state
MMU/TLB  = virtual-address translation + translation cache
L1/L2/L3 = hot instruction/data caches
```

## Cache

```text
Registers → L1 → L2 → L3 → RAM
fast/small                 slower/larger
```

- L1 often split into **L1I** (instructions) and **L1D** (data).
- CPU caches blocks called **cache lines** (often 64 B).
- Multiple cores caching the same line need **cache coherence**.
- Write on one core can invalidate/transfer other cores' copies.

## NUMA

```text
core on node 0 → RAM 0 = local
core on node 0 → RAM 1 = remote / usually slower
```

**NUMA = not all RAM is equally close to all CPUs.**

## Instruction Lifecycle

```text
PC → FETCH → DECODE → EXECUTE → optional MEMORY → WRITEBACK
```

- next sequential instruction is often already hot in L1I,
- branch/call can jump elsewhere and cause cache/iTLB misses,
- inlining trades fewer calls for larger code.

## Keep CPU Busy

```text
Pipelining  = overlap stages of different instructions
Parallelism = different work on different physical cores
SMT         = multiple hardware threads share one physical core
SIMD        = one instruction operates on multiple data elements
```

## TLB

```text
virtual page → TLB → physical frame
```

ASID/PCID-style tags help translations from different address spaces coexist.

## RISC / CISC

```text
RISC teaching model → simple/regular instructions
CISC teaching model → richer/complex instructions
```

No “1 instruction = 1 cycle” guarantee on modern CPUs.

## Workloads

```text
CPU-bound = time spent computing
I/O-bound = time spent waiting on disk/network/DB/etc.
```

`top`:

```text
us = user CPU
sy = kernel CPU
id = idle
wa = iowait (mostly block-I/O accounting)
```

**100% utilization ≠ necessarily saturation.** Check whether work is waiting.

## Bridge to Next Lecture

> Processes/threads are how software gives the scheduler independent execution streams that can use CPU cores while other streams compute or wait.
