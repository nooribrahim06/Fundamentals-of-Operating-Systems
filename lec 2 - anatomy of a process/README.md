# 02 — The Anatomy of a Process

> **Goal:** understand what a process actually is, how its instructions reach the CPU, and why the process is split into **text/code, data, heap, and stack**.
>
> This README follows the lecture in order and keeps the low-level tracing/code because that is the point of this section.

<p align="center">
  <img src="imgs/generated/process-anatomy-map.svg" alt="One connected map of executable, process address space, CPU registers, and kernel metadata" width="95%">
</p>

---

## Lecture Map

| Part | What you should be able to answer |
|---|---|
| Program vs Process | What changes when an executable starts running? |
| CPU execution | How does the CPU know which instruction to execute next? |
| Text / Code | Where are the machine instructions? |
| Stack | How are function calls and local variables managed? |
| Data / Static | Where do global/static variables live? |
| Heap | Where do dynamic allocations live and how do pointers reach them? |
| Linux demo | Can we see these regions in a real process? |

> **Slide bank:** every course slide from **26 through 121** is included under `imgs/slides/`. The README embeds the slides that materially help the explanation; the full range is still available in the repo.

---

# 0. Before the process: why C was chosen

The previous section established that an operating system is extremely useful, but conceptually software **can** be written to interact directly with hardware. The tradeoff is complexity: you would have to handle hardware details and resource management yourself.

This section moves to the kernel's main unit of execution: the **process**.

The instructor uses **C** because the relationship between the code and native CPU instructions is easier to expose:

```text
C source
   ↓ compile + link
native executable
   ↓ run
process
   ↓
CPU instructions
```

With a runtime-based language, another large layer exists:

```text
Java source               Python source
    ↓                          ↓
bytecode                    CPython/runtime
    ↓                          ↓
JVM / JIT                  native runtime code
    ↓                          ↓
native instructions        CPU
```

### Runtime — the definition we need

> **Runtime = software/environment that remains involved while your language-level program executes and provides machinery the language depends on.**

Examples in the lecture discussion:

- Java → JVM, JIT, garbage collection, runtime services.
- Python → a runtime such as CPython interprets/executes the Python program.
- C/Rust → commonly compiled ahead of time to native machine code, making this particular CPU walkthrough easier to visualize.

The instructor also uses a proxy-performance story to motivate why runtime behavior, garbage collection, instruction execution, and low-level latency can matter in performance-sensitive systems. Treat that story as a **motivation example from the lecture**, not as a rule that one language is always faster than another.

---

# 1. Program vs Process

<p align="center">
  <img src="imgs/slides/os_page-0027.png" alt="Course slide: Program" width="48%">
  <img src="imgs/slides/os_page-0028.png" alt="Course slide: Process" width="48%">
</p>

## Program

A **program** is the executable artifact at rest.

It has already been compiled and linked for a CPU architecture and follows an executable file format.

Examples:

```text
postgres
nginx
my_server
a.out
```

On Linux, the lecture introduces **ELF** as the executable format.

The executable needs structure because the loader must know things such as:

- where executable code is,
- where program data is,
- what the entry point is,
- which external libraries are required,
- how the file's pieces should be mapped when the program runs.

## Process

> **Process = a program in motion.**

When the program is run, a process is created with live state:

- a **PID**,
- a current instruction position,
- CPU register state,
- process memory,
- stack,
- heap,
- data/static mappings,
- kernel-managed metadata.

One executable can create several separate processes:

<p align="center">
  <img src="imgs/slides/os_page-0029.png" alt="Same postgres program producing multiple processes with different PIDs and program counters" width="72%">
</p>

Each can be at a different instruction and contain different runtime data.

---

# 2. Compile vs Link

<p align="center">
  <img src="imgs/slides/os_page-0030.png" alt="Producing machine code slide" width="48%">
  <img src="imgs/slides/os_page-0031.png" alt="Assembly and machine code slide" width="48%">
</p>

## Compilation

The compiler translates source code into lower-level code/object pieces for the target architecture.

Simplified teaching path:

```text
C code
  ↓
assembly
  ↓
machine code
```

Assembly is **not** machine code; the lecture uses readable assembly-like instructions to represent what machine instructions are doing.

Example teaching translation:

```c
int a = 1;
int b = 3;
int c = a + b;
```

Conceptually:

```asm
mov r0, 1
mov r1, 3
add r3, r0, r1
store r3, [c]
```

Compilers can optimize. For example, if `a` and `b` never need to exist as independently addressable memory values, the compiler may keep values in registers rather than performing unnecessary RAM writes.

## Linking

Real programs depend on multiple compiled objects and libraries.

```text
my_code.o ───────┐
lib/object A ─────┼── linker ──► executable
lib/object B ─────┘
```

### Static linking

More required code is incorporated into one executable.

**Benefit:** fewer external runtime library dependencies.

**Tradeoff:** larger executable.

### Dynamic linking

The executable keeps references to shared libraries that are loaded/mapped when the process starts.

This explains the classic error:

```text
program copied successfully
BUT
required dynamic library is missing
```

Windows commonly exposes this idea with DLLs; Linux commonly uses shared objects such as `.so` files.

## CPU architecture matters

Native instructions are architecture-specific:

```text
compile for ARM   → ARM machine instructions
compile for x86   → x86 machine instructions
```

An executable built for one instruction-set architecture cannot simply be assumed to execute natively on another.

---

# 3. Process identity: PID, PCB, and namespaces

## PID

Every running process receives a **process ID (PID)**.

The PID gives the system a way to refer to that running process.

## PCB — Process Control Block

The lecture uses **PCB** as the mental model for the kernel-side process record.

> **PCB = the process's kernel-side ID card / saved state.**

Conceptually it contains information such as:

```text
PCB
├── PID
├── process state
├── saved CPU register state
├── program-counter state
├── scheduling information
├── memory-management information
├── open-file information
└── accounting/statistics
```

The important point is not the exact fields of a specific OS kernel. The important point is:

> The kernel needs a data structure describing the process so it can stop it, schedule another one, and later continue it correctly.

## PID namespaces — preview of containers

The lecture briefly previews namespaces.

A container can see its own PID view:

```text
Container A: PID 700
Container B: PID 700
Host view: kernel still distinguishes the actual tasks
```

The same broader isolation idea exists for other resource views, such as networking and mounts/storage.

This is one of the kernel mechanisms container systems build upon.

---

# 4. CPU architecture needed for this lecture

You do **not** need a full CPU-design course here. We only need the pieces that participate directly in process execution.

```text
CPU
└── Core
    ├── general registers (R0, R1, ...)
    ├── PC — Program Counter
    ├── SP — Stack Pointer
    ├── BP/FP — Base/Frame Pointer
    ├── return/link information
    ├── instruction decoder / control logic
    └── ALU
```

## Core

A CPU can contain multiple cores. The lecture's simplified scheduling model is:

> a logical execution context runs one instruction stream at a time, and the OS schedules processes/threads onto available logical CPUs.

The instructor briefly mentions **Hyper-Threading / simultaneous multithreading** as the exception to the oversimplified "one physical core = one running process" picture: one physical core can expose more than one logical CPU execution context and share some internal resources.

## Decoder / control unit

This is the part you remembered from the microprocessor course.

```text
machine instruction bits
        ↓
instruction decoder
        ↓
"this means ADD / LOAD / STORE / BRANCH"
        ↓
control signals
        ↓
ALU / registers / memory unit
```

The decoder determines **what the instruction means**. The ALU performs arithmetic/logic work such as addition, subtraction, AND, OR, comparisons, etc.

---

# 5. Why caches suddenly matter

<p align="center">
  <img src="imgs/generated/cpu-memory-hierarchy.svg" alt="CPU registers, cache, RAM and storage latency hierarchy" width="95%">
</p>

The lecture uses these approximate teaching values:

<p align="center">
  <img src="imgs/slides/os_page-0041.png" alt="Course slide showing approximate register, cache, RAM, SSD, and HDD access times" width="72%">
</p>

```text
Register access  ~ 1 ns
L1 cache         ~ 2 ns
L2 cache         ~ 7 ns
L3 cache         ~ 15 ns
Main memory      ~ 100 ns
SSD              ~ 150 μs
HDD              ~ 10 ms
```

These are illustrative values, not constants for every machine.

The lesson is the ordering:

```text
FASTEST / TINIEST
Registers
   ↓
L1
   ↓
L2
   ↓
L3
   ↓
RAM
   ↓
SSD/HDD
SLOWER / LARGER
```

## Why nearby memory is valuable

The CPU does not normally fetch one lonely byte and forget everything around it. Memory is moved in blocks/cache lines.

So if data is laid out like:

```text
[A][B][C][D]
```

and `A` causes a memory fetch, nearby values can arrive in cache too:

```text
A → cache miss / fetch
B → likely cache hit
C → likely cache hit
D → likely cache hit
```

This principle — **spatial locality** — explains a huge amount of the stack/data performance discussion later.

---

# 6. The Program Counter and one instruction cycle

The **Program Counter (PC)** / instruction pointer tells the CPU where instruction execution should continue.

<p align="center">
  <img src="imgs/slides/os_page-0043.png" alt="Program Counter points at the first instruction" width="48%">
  <img src="imgs/slides/os_page-0044.png" alt="Fetch instruction into instruction register" width="48%">
</p>

A simplified cycle:

```text
PC says where
    ↓
FETCH instruction
    ↓
DECODE instruction
    ↓
EXECUTE
    ↓
advance/replace PC as required
    ↓
repeat
```

## Step 1 — start

The executable contains entry-point information. When the process begins, execution is arranged to begin at that entry code.

```text
PC → first instruction address
```

## Step 2 — fetch

The instruction is fetched from memory/cache into CPU-side instruction machinery.

<p align="center">
  <img src="imgs/slides/os_page-0044.png" alt="Fetch instruction slide" width="72%">
</p>

## Step 3 — decode

The CPU recognizes the opcode and operands.

Example:

```text
opcode → ADD
inputs → R0, R1
output → R3
```

## Step 4 — execute

<p align="center">
  <img src="imgs/slides/os_page-0045.png" alt="Execute instruction slide" width="72%">
</p>

The required CPU unit performs the work.

## Step 5 — continue

<p align="center">
  <img src="imgs/slides/os_page-0046.png" alt="Increment Program Counter slide" width="72%">
</p>

The slides use a fixed-size teaching example where the next instruction is four bytes away.

**Do not memorize `PC += 4` as a universal law.** The exact instruction length/PC behavior depends on the ISA and instruction.

## Cache effect on instruction fetching

<p align="center">
  <img src="imgs/slides/os_page-0047.png" alt="Next instruction and CPU cache note" width="72%">
</p>

The lecture intentionally first draws the next fetch as another RAM access, then points out that nearby instructions often arrived in the cache with the earlier fetch.

So sequential code can become:

```text
first instruction  → memory/cache miss
next instruction   → L1 instruction-cache hit
next instruction   → hit
next instruction   → hit
```

## Addition and store

<p align="center">
  <img src="imgs/slides/os_page-0051.png" alt="Execute add slide" width="48%">
  <img src="imgs/slides/os_page-0053.png" alt="Store result back to memory slide" width="48%">
</p>

The final result may have to be written back out of CPU registers into process memory.

That is a **store** operation.

---

# 7. Process memory layout

<p align="center">
  <img src="imgs/slides/os_page-0028.png" alt="Process memory layout from course slide" width="72%">
</p>

The lecture's conceptual process layout:

```text
HIGH ADDRESS

┌──────────────────────┐
│       STACK ↓        │
│                      │
│                      │
│        HEAP ↑        │
├──────────────────────┤
│    DATA / STATIC     │
├──────────────────────┤
│     TEXT / CODE      │
└──────────────────────┘

LOW ADDRESS
```

The easiest way to remember the regions:

```text
TEXT  → what instructions exist?
DATA  → what fixed/global/static data exists?
HEAP  → what dynamic allocations exist?
STACK → what function-call state exists right now?
```

## Important address simplification in this lecture

The instructor deliberately postpones virtual memory and initially speaks as if process addresses directly correspond to physical memory.

That is a **teaching simplification**.

Modern general-purpose OS processes normally operate using **virtual addresses**. The next course section explains how those are mapped to physical memory.

The transcript also previews that a DRAM physical address is eventually interpreted by memory-controller hardware into internal DRAM structure such as bank/row/column fields. That is a hardware detail hidden behind the address abstraction and is not developed further in this lecture.

---

# 8. Text / Code section

The **text/code section** contains machine instructions.

```text
TEXT / CODE
0x....  instruction
0x....  instruction
0x....  instruction
```

The PC points into executable code.

Properties emphasized in the lecture:

- contains native instructions for the target CPU,
- mapped when the process begins,
- normally treated as executable/read-only program code,
- sequential instruction locality can help the instruction cache.

The instructor briefly contrasts this with JIT runtimes, where code can be generated during runtime instead of all native code being fixed ahead of time.

---

# 9. The Stack — function execution memory

> **Best mental model: stack = function-call workspace/history.**

<p align="center">
  <img src="imgs/slides/os_page-0056.png" alt="Stack slide showing local variables and frames" width="72%">
</p>

The lecture emphasizes:

- functions have local variables,
- each active call gets a **stack frame**,
- the stack grows from high addresses toward low addresses in this model,
- stack space is limited,
- stack data is tightly packed, which gives good locality.

## A stack frame

```text
main()
┌─────────────────┐
│ local a         │
│ local b         │
│ local c         │
│ saved call info │
└─────────────────┘
```

If `main()` calls `func1()`:

```text
┌─────────────────┐
│ main frame      │
├─────────────────┤
│ func1 frame     │ ← active
└─────────────────┘
```

If `func1()` returns, its frame is unwound and `main` becomes active again.

---

## 9.1 Stack Pointer — SP

<p align="center">
  <img src="imgs/slides/os_page-0057.png" alt="Stack Pointer slide" width="72%">
</p>

**SP = Stack Pointer**, a CPU register used to track the active end of the stack.

The lecture's simplified allocation operation:

```text
SP = 1024
need 12 bytes
SP = SP - 12
```

Now that range is available to the new frame.

Deallocation can be as cheap as moving SP back:

```text
SP = SP + 12
```

The old bytes do not need to be individually erased. They are simply no longer part of the active frame and can later be overwritten.

That is one reason stack allocation/deallocation is so cheap.

---

## 9.2 Base Pointer / Frame Pointer — BP / FP

<p align="center">
  <img src="imgs/slides/os_page-0058.png" alt="Base Pointer slide" width="72%">
</p>

Problem: **SP moves**.

If local-variable addresses were always described relative to the moving SP, keeping track of offsets can become inconvenient.

The lecture introduces a more stable frame reference:

```text
BP → start/reference point for current frame
```

Example:

```text
BP
│
├── BP - 4  → local b
├── BP - 8  → local c
└── ...
```

So the mental distinction is:

```text
SP → where does the active stack end?
BP → stable reference for this function's frame
```

Compilers/ABIs can optimize or omit a dedicated frame pointer, but this is the model used for understanding the mechanics.

---

## 9.3 Nested calls — the first problem

<p align="center">
  <img src="imgs/slides/os_page-0059.png" alt="Nested call slide" width="48%">
  <img src="imgs/slides/os_page-0063.png" alt="Saving old base pointer for nested call" width="48%">
</p>

When `main` calls `func1`, BP must change to describe `func1`.

But if we overwrite the old BP forever, how can we reconstruct `main` when `func1` returns?

Answer:

> **save the old BP in the new call frame before replacing it.**

Then returning can restore it.

<p align="center">
  <img src="imgs/slides/os_page-0064.png" alt="Restoring old base pointer" width="48%">
  <img src="imgs/slides/os_page-0065.png" alt="Deallocating function frame by moving SP" width="48%">
</p>

---

## 9.4 Return address / Link Register — the second problem

We restored the old stack frame — but there is another question:

> **Where inside `main` should execution continue?**

<p align="center">
  <img src="imgs/slides/os_page-0066.png" alt="Return address and Link Register slide" width="72%">
</p>

A function call causes execution to jump to different instructions.

```text
main code
   ↓ CALL
func1 code
   ↓ RETURN
??? where in main?
```

So the machine preserves a **return address**.

The ARM-flavored model used in the lecture introduces **LR — Link Register** for this.

```text
PC → where execution is now
LR → where this function should return
```

Together:

```text
PC → instruction position
SP → stack end
BP → current frame reference
LR → return location
```

<p align="center">
  <img src="imgs/generated/function-call-flow.svg" alt="Function call flow showing PC, SP, BP and return address" width="95%">
</p>

---

# 10. Full `main → func1 → main` execution trace

This is the most detailed part of the lecture. Do not memorize every number. Follow **what problem each save/restore operation solves**.

Example source used by the lecture:

```c
void func1(void) {
    int z = 1;
    z = z + 1;
}

int main(void) {
    int a = 1;
    int b = 3;

    func1();

    int c = a + b;
    return c;
}
```

Repo copy: [`examples/stack_call_demo.c`](examples/stack_call_demo.c)

### Trace summary

| Step | What happens | Why |
|---|---|---|
| 1 | Load process code | CPU needs instructions in memory |
| 2 | Initialize SP/BP/LR/PC state | establish execution state |
| 3 | Enter `main`, move SP | allocate `main` frame |
| 4 | Save previous BP/LR | allow eventual return out of `main` |
| 5 | Set `main` BP | stable reference for locals |
| 6 | Put `1` in register, store `a` | initialize local |
| 7 | Put `3` in register, store `b` | initialize local |
| 8 | Save next-main instruction as return address | `func1` must know where to return |
| 9 | PC jumps to `func1` code | begin callee |
| 10 | Move SP / allocate `func1` frame | space for callee state |
| 11 | Save `main` BP | restore caller frame later |
| 12 | Set new BP for `func1` | stable callee frame |
| 13 | `z = 1`, then `z = z + 1` | execute callee body |
| 14 | Restore `main` BP | callee is ending |
| 15 | Pop/deactivate `func1` frame | SP returns to caller position |
| 16 | Set PC from return address | resume `main` |
| 17 | Reload needed `a`/`b` values | callee may have changed registers |
| 18 | Add values and store `c` | finish main computation |
| 19 | Restore previous state and return | `main` ends |

<details>
<summary><strong>Open the slide-by-slide trace (course slides 69–91)</strong></summary>

<br>

<p align="center">
  <img src="imgs/slides/os_page-0069.png" alt="Trace example source and code addresses" width="48%">
  <img src="imgs/slides/os_page-0072.png" alt="Main called and stack frame allocated" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0073.png" alt="Save registers in main frame" width="48%">
  <img src="imgs/slides/os_page-0074.png" alt="Set main base pointer" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0075.png" alt="Set a value in register" width="48%">
  <img src="imgs/slides/os_page-0076.png" alt="Store a into stack memory" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0077.png" alt="Set and store b" width="48%">
  <img src="imgs/slides/os_page-0078.png" alt="Prepare return address before calling func1" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0079.png" alt="Jump program counter to func1" width="48%">
  <img src="imgs/slides/os_page-0080.png" alt="Store main base pointer in func1 frame" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0081.png" alt="Set base pointer for func1" width="48%">
  <img src="imgs/slides/os_page-0082.png" alt="Set z equal to 1" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0083.png" alt="Add one to z" width="48%">
  <img src="imgs/slides/os_page-0084.png" alt="Store z back to stack memory" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0085.png" alt="Restore main base pointer before exiting func1" width="48%">
  <img src="imgs/slides/os_page-0086.png" alt="Return to main and restore variables" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0087.png" alt="Restore b" width="48%">
  <img src="imgs/slides/os_page-0088.png" alt="Compute c equals a plus b" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0089.png" alt="Store c in memory" width="48%">
  <img src="imgs/slides/os_page-0090.png" alt="Terminate main and restore previous state" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0091.png" alt="Stack pointer restored after main ends" width="48%">
</p>

</details>

---

# 11. Function-call cost and instruction-cache locality

The purpose of the long trace is not to scare you away from functions.

It is to show that a function call is not literally free:

```text
call function
   ↓
manage call/return state
manage frame/registers as required
PC jumps to another code location
execute
restore state
return
```

The instructor focuses especially on the **instruction cache**.

Sequential code:

```text
instruction A
instruction B
instruction C
instruction D
```

has strong locality.

Deeply bouncing through code:

```text
main → A → B → C → main → D → ...
```

can cause the PC to move among distant code regions and potentially create more instruction-cache misses.

## Inlining

A compiler may replace a small call with the function's instructions directly inside the caller.

```text
normal call:
main → func → main

inline:
main instructions + copied func instructions + main instructions
```

Potential benefit:

- less call machinery,
- better straight-line execution/locality.

Tradeoff:

- repeated copies enlarge the binary/text section,
- larger code can itself pressure instruction caches.

## MySQL performance tangent from the lecture

The instructor presents a **speculative performance theory** around larger codebases / increased function calling and instruction-cache behavior when discussing MySQL versions. The important lesson to preserve is not a claim that this explains every observed MySQL difference; it is the performance mechanism:

> **more code + more control-flow jumps can create instruction-cache pressure.**

## SIMD tangent

During the trace, the instructor briefly contrasts ordinary scalar instructions with **SIMD — Single Instruction, Multiple Data**.

Conceptually:

```text
scalar:
add one pair
add next pair
add next pair

SIMD:
one instruction operates on a vector/lane set of multiple values
```

This is a CPU-performance aside, not part of stack semantics.

---

# 12. Stack overflow

<p align="center">
  <img src="imgs/slides/os_page-0092.png" alt="Stack overflow slide" width="72%">
</p>

Stack space is finite.

Two important ways to exhaust it:

### 1. Unbounded/deep recursion

```c
void f(void) {
    f();
}
```

Each unresolved call keeps adding call state/frames.

### 2. Very large local variables

```c
void f(void) {
    char huge[VERY_LARGE_SIZE];
}
```

Large locals consume stack space quickly.

The stack-size limit can be configured/affected by environment, OS/runtime/compiler/toolchain choices depending on platform.

---

# 13. Parameters and return values

The detailed slide trace intentionally avoids much parameter complexity, but the transcript adds the idea:

- calling conventions commonly place initial parameters in designated registers,
- additional parameters may use the stack,
- return values commonly use a designated return register,
- exact registers/rules are defined by the platform's ABI/calling convention.

So a function call may also require preparing argument registers and preserving values the caller still needs.

---

# 14. Data / Static section

<p align="center">
  <img src="imgs/slides/os_page-0095.png" alt="Data section slide" width="72%">
</p>

The data/static area stores program data whose existence/layout is known independently of individual function calls.

The lecture focuses on:

- global variables,
- static variables,
- read-only data / constants,
- writable initialized/uninitialized static-storage data,
- a mostly fixed layout compared with the dynamically changing heap.

Example:

```c
int A = 10;
int B = 20;

int main(void) {
    int sum = A + B;
    return sum;
}
```

Repo copy: [`examples/data_section_demo.c`](examples/data_section_demo.c)

The locals belong to a stack frame, while `A` and `B` have static storage duration.

## Why direct offsets are easy

If the start/base of the relevant data mapping is known, globals can be referenced at known offsets:

```text
data_base + offset(A)
data_base + offset(B)
```

Unlike a local variable in constantly appearing/disappearing stack frames, the global's relative location is stable for that program image.

## Data trace

<p align="center">
  <img src="imgs/slides/os_page-0097.png" alt="Start process with data section initialized" width="48%">
  <img src="imgs/slides/os_page-0098.png" alt="Load global A" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0099.png" alt="Load global B, likely cached after A" width="48%">
  <img src="imgs/slides/os_page-0100.png" alt="Add globals and store local sum" width="48%">
</p>

The performance point is spatial locality:

```text
read A → fetch nearby cache line
read B → B may already be in L1
```

## Writes and concurrency

Read-only/shared data is straightforward.

Writable shared state becomes more complicated when multiple execution contexts operate on it:

```text
Core 0 / Thread A → writes global X
Core 1 / Thread B → has cached X
```

Hardware must maintain cache coherence, so writes can cause invalidation/coherence traffic. At the software level, concurrent writes can also require synchronization to avoid races.

The lecture mentions mutexes as the kind of mechanism needed later when shared writable state is accessed concurrently.

---

# 15. The Heap — dynamic process memory

<p align="center">
  <img src="imgs/generated/stack-heap-lifetime.svg" alt="Stack pointer pointing to dynamically allocated heap object and lifetime difference" width="95%">
</p>

<p align="center">
  <img src="imgs/slides/os_page-0103.png" alt="Heap section slide" width="48%">
  <img src="imgs/slides/os_page-0104.png" alt="Pointer slide" width="48%">
</p>

> **Best mental model: heap = dynamic allocation area whose objects are not tied automatically to the lifetime of one stack frame.**

The lecture emphasizes:

- stores dynamically allocated data,
- useful for larger/dynamic objects,
- allocation lifetime can outlive the function that requested it,
- accessed through addresses/pointers/references,
- conceptually grows low → high in the simplified diagram,
- C APIs: `malloc()` and `free()`,
- other languages may use constructs such as `new` plus runtime memory management.

## Pointer ≠ heap object

This is the single most important heap picture:

```text
STACK                          HEAP

ptr = 0x9000  ───────────────► [allocated object at 0x9000]
```

`ptr` is just a value containing an address.

The pointer itself may live in:

- the stack,
- the data/static area,
- another heap object.

A pointer's type tells the compiler how the pointed-to bytes should be interpreted and how pointer arithmetic/accesses are treated.

---

# 16. `malloc()` / heap trace

Example matching the lecture's heap sequence:

```c
#include <stdlib.h>

int main(void) {
    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) return 1;

    *ptr = 10;
    *ptr = *ptr + 1;

    free(ptr);
    return 0;
}
```

Repo copy: [`examples/heap_demo.c`](examples/heap_demo.c)

### Step-by-step

| Step | What happens |
|---|---|
| 1 | `main` creates local pointer storage |
| 2 | requested allocation size is passed to `malloc` according to calling convention |
| 3 | execution enters allocator/library code |
| 4 | allocator obtains/manages a heap block |
| 5 | allocator returns the block's address |
| 6 | that address is stored in `ptr` |
| 7 | `*ptr = 10` writes through the pointer into heap memory |
| 8 | `*ptr = *ptr + 1` reads/modifies the heap value |
| 9 | address is passed to `free` |
| 10 | allocator marks/releases the block for reuse |
| 11 | `main` returns |

<details>
<summary><strong>Open the slide-by-slide malloc/free trace (slides 106–114)</strong></summary>

<br>

<p align="center">
  <img src="imgs/slides/os_page-0106.png" alt="Heap trace starts with malloc and free code mapped" width="48%">
  <img src="imgs/slides/os_page-0107.png" alt="Initialize pointer in main stack" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0108.png" alt="Call malloc and save caller state" width="48%">
  <img src="imgs/slides/os_page-0109.png" alt="Malloc uses argument and returns heap address" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0110.png" alt="Malloc returns and pointer stores address" width="48%">
  <img src="imgs/slides/os_page-0111.png" alt="Write value through pointer to heap" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0112.png" alt="Increment value stored in heap" width="48%">
  <img src="imgs/slides/os_page-0113.png" alt="Pass pointer to free" width="48%">
</p>
<p align="center">
  <img src="imgs/slides/os_page-0114.png" alt="Free returns and main returns" width="48%">
</p>

</details>

---

# 17. Heap memory bugs

## 17.1 Memory leak

<p align="center">
  <img src="imgs/slides/os_page-0115.png" alt="Memory leak slide" width="72%">
</p>

A classic C-style leak:

```text
stack pointer variable ─────► heap block

function returns
pointer disappears

heap block remains allocated
```

So:

> **Leak = allocated memory remains, but the program loses the path/reference needed to release it.**

Long-running servers make leaks particularly dangerous because the leaked amount can accumulate request after request.

The lecture notes that higher-level runtimes can use mechanisms such as **reference counting** and/or **garbage collection** to reclaim unreachable heap objects automatically.

## 17.2 Dangling pointer

<p align="center">
  <img src="imgs/slides/os_page-0116.png" alt="Dangling pointer slide" width="72%">
</p>

```text
pointer ─────► object
free(object)
pointer still contains old address
```

Now the pointer refers to memory that is no longer a valid owned object.

Possible result:

- invalid data,
- memory corruption,
- undefined behavior,
- segmentation fault.

> **Dangling pointer = pointer still exists after the pointed-to allocation has been freed.**

## 17.3 Double free

<p align="center">
  <img src="imgs/slides/os_page-0117.png" alt="Double-free slide" width="72%">
</p>

```c
free(ptr);
free(ptr);   // same allocation again
```

> **Double free = releasing the same allocation more than once.**

This can crash the program or corrupt allocator state.

### Three-line memory rule

```text
LEAK         → memory alive, reference lost
DANGLING     → reference alive, memory freed
DOUBLE FREE  → same memory freed twice
```

---

# 18. Stack vs Heap performance

<p align="center">
  <img src="imgs/slides/os_page-0118.png" alt="Stack vs heap performance slide" width="72%">
</p>

The lecture's shorthand:

```text
STACK
- built-in frame discipline
- allocation/deallocation often just pointer movement
- locals frequently close together
- limited space
- strong locality

HEAP
- dynamic allocator must manage blocks
- allocation/free pattern can scatter objects
- objects can be reached through pointers
- locality is less predictable
```

Do not turn this into the bad rule:

> `stack = always fast`, `heap = always slow`.

The useful rule is:

> **Stack management is extremely regular; heap management is flexible and therefore requires more bookkeeping and can produce less predictable locality.**

---

# 19. Escape analysis

<p align="center">
  <img src="imgs/slides/os_page-0119.png" alt="Escape analysis slide" width="72%">
</p>

The lecture introduces **escape analysis** as a compiler/runtime optimization used by languages such as Go and Java.

Question:

> Does this object need to survive outside the current call / does its reference escape?

If not, an implementation may be able to avoid a normal long-lived heap allocation and use a cheaper placement strategy.

The point is not to memorize one language's exact rules — it is to understand that compilers can use lifetime analysis to optimize allocation decisions.

---

# 20. Program break — `brk` / `sbrk`

<p align="center">
  <img src="imgs/slides/os_page-0120.png" alt="Program break slide" width="72%">
</p>

The lecture introduces the **program break** as the traditional boundary associated with the process's heap area.

Historically, Unix-like systems provide APIs such as:

```text
brk
sbrk
```

for moving that boundary.

The instructor's practical advice is not to manually build application allocation logic around them; normal programs use memory allocators such as `malloc`, which manage allocation policy for you and may use multiple OS mechanisms underneath.

---

# 21. Kernel code space — where is the OS while this happens?

A question that came up during the lecture:

> **Where is kernel code?**

The kernel is itself compiled software. Its machine instructions are resident/mapped in protected kernel memory while the system runs.

Conceptually:

```text
CPU core
  │
  ├── user mode   → execute your process's code
  │
  ├── system call / interrupt / exception
  ▼
  ├── kernel mode → execute kernel instructions
  │
  └── return      → continue user process
```

The same CPU core can execute both user and kernel instructions at different times. The protection boundary is about privilege/memory access, not a separate physical CPU.

This also explains why the process's saved execution state matters: when the CPU stops executing one context and later returns to it, the system needs enough state to resume correctly.

---

# 22. Practical demo 1 — compile and inspect the PC with GDB

The transcript uses a small C program and GNU tools.

Repo version: [`examples/process_demo.c`](examples/process_demo.c)

```c
#include <stdio.h>

int main(void) {
    int a = 1;
    int b = 2;
    int c = a + b;

    printf("a + b = %d\n", c);
    c = c + 1;

    return 0;
}
```

## Check kernel/system

```bash
uname
uname -a
```

## Compile normally

```bash
gcc process_demo.c -o process_demo
```

## Generate assembly

```bash
gcc -S process_demo.c -o process_demo.s
cat process_demo.s
```

The purpose is not to understand every assembly instruction yet. Look for the connection:

```text
C source
   ↓ compiler
assembly/native instruction sequence
```

## Compile with debug symbols

```bash
gcc -g process_demo.c -o process_demo
```

## Start GDB

```bash
gdb ./process_demo
```

Inside GDB:

```gdb
start
info registers
next
info registers
```

Watch the program counter/instruction-pointer register change as execution advances.

You can also attach to an already-running process:

```bash
ps
sudo gdb -p <PID>
```

<p align="center">
  <img src="imgs/slides/os_page-0034.png" alt="Course demo slide for process and debugger" width="72%">
</p>

---

# 23. Practical demo 2 — see the real process mappings

The final lecture demo creates a long-running C process containing examples of:

- initialized/static program data,
- stack state,
- a large `malloc` allocation,
- long-running work so the process remains alive long enough to inspect.

Then Linux is asked to show the mappings.

## `/proc`

Linux exposes live process/system information through the virtual `/proc` filesystem.

```text
/proc/<PID>/...
```

This is not simply a normal directory full of persistent disk files. It is an interface exposing kernel-maintained information.

## View mappings

```bash
cat /proc/<PID>/maps
```

A simplified output shape:

```text
address-range   perms   mapped object

...             r-xp    /home/pi/highcpu
...             r--p    /home/pi/highcpu
...             rw-p    /home/pi/highcpu

...             rw-p    [heap]

...             r-xp    libc.so
...             r--p    libc.so
...             rw-p    libc.so

...             rw-p    [stack]
```

### Permissions

```text
r → readable
w → writable
x → executable
```

So an executable code mapping might appear as something like:

```text
r-x
```

while writable data commonly looks like:

```text
rw-
```

## What the mappings demonstrate

### Executable code

The program's text/code is mapped into the process address space.

### Program data

Read-only and writable program data appears in separate mappings/permission regions.

### Heap

`[heap]` is dynamic process memory rather than a pre-existing file section containing all future heap objects.

### Stack

`[stack]` is the process's active stack mapping.

### libc / shared libraries

Functions such as `malloc` and `printf` can come from dynamically mapped shared-library code such as libc.

That is why the process mapping contains more executable code than only the bytes of your own source program.

## Why tiny data can still occupy 4 KiB mappings/pages

The demo notices mappings with a 4 KiB granularity even though the program may contain only a tiny amount of data.

That previews the next section:

> virtual memory is managed in **pages**, and the common page size used in the lecture example is **4 KiB**.

So requesting/storing one integer does not imply the OS creates a one-byte or four-byte virtual-memory mapping exclusively for it.

## Demo sizes are not universal

The instructor observes stack/heap sizes around the values shown on his Raspberry Pi process. Those are measurements of that demo process, **not fixed stack/heap sizes for all Linux processes**.

---

# 24. `top`, `htop`, and the kernel vs OS tooling

The lecture also uses:

```bash
top
htop
```

These are **userspace utilities** that read system/process information and present it nicely.

This reinforces the previous lecture's distinction:

```text
Linux kernel
    ↓ exposes information
/proc and other kernel interfaces
    ↓
userspace tools
    ↓
top / htop / ps / debuggers
```

The OS experience is more than only the kernel; distributions bundle the kernel with tooling, shells, package systems, UI, and utilities.

---

# 25. How all four process regions work together

Consider:

```c
int global_limit = 100;          // data/static

void process_user(void) {
    int local_count = 3;         // stack
    int *buffer = malloc(4096);  // pointer on stack, allocation on heap

    /* work */                   // instructions in text/code

    free(buffer);
}
```

Map it:

```text
TEXT / CODE
└── instructions for process_user(), malloc call, loop, free call...

DATA / STATIC
└── global_limit = 100

STACK
└── process_user frame
    ├── local_count = 3
    └── buffer = address 0x....

HEAP
└── 4096-byte allocation
```

That is the anatomy of the process in one example.

---

# 26. Backend-engineering connections

This is why an OS/process lecture matters to a future backend developer.

## Long-running servers and leaks

A CLI program may leak memory and exit two seconds later.

A backend can run for weeks:

```text
request → leak
request → leak
request → leak
...
RAM usage keeps growing
```

Understanding heap lifetime makes production memory graphs less mysterious.

## Function calls and hot paths

Most functions are completely fine. But when optimizing a tiny latency-sensitive hot path — a proxy, parser, database inner loop, serialization routine — control-flow, allocation, and cache behavior can matter.

## Runtime behavior

A Java/Python/Node backend is still ultimately running as native process(es) whose runtime interacts with:

- CPU scheduling,
- virtual memory,
- stack/heap allocations,
- system calls,
- sockets,
- files.

The high-level language does not make the OS disappear; it adds abstractions above it.

---

# 27. Lecture simplifications to keep straight

These do **not** replace the lecture model; they prevent the diagrams from becoming false rules.

| Lecture picture | Keep this clarification in mind |
|---|---|
| PC jumps by 4 | That is the example ISA/instruction size, not universal |
| stack grows down / heap grows up | Common conceptual virtual-layout picture, not a promise about every OS/architecture/allocation |
| heap is “random” | Means dynamic allocations can be scattered/less predictable, not literally random |
| all functions can access heap | A function needs a pointer/reference/path to the allocation |
| BP always exists | Optimizing compilers can omit/use frame pointers differently |
| LR is the return address register | This is ARM-style; other ISAs/calling conventions store return information differently |
| process addresses shown as physical | Lecture postpones virtual memory; real general-purpose processes normally use virtual addresses |

---

# 28. One-minute revision

## Program vs Process

```text
Program → executable at rest
Process → running instance with live state
```

## Build

```text
Compiler → source → machine/object code
Linker   → connect objects/libraries → executable
```

## CPU

```text
PC → where execution continues
Decoder → what instruction means
ALU → arithmetic / logic
Registers → immediate working state
```

## Process memory

```text
TEXT  → instructions
DATA  → globals/statics
HEAP  → dynamic allocation
STACK → function frames/locals/call state
```

## Stack registers

```text
SP → stack end
BP → current frame reference
LR / return address → where callee returns
PC → code location
```

## Heap bugs

```text
Leak        → allocation alive, reference lost
Dangling    → reference alive, allocation freed
Double free → same allocation freed again
```

## Linux inspection

```bash
ps
top
htop
gdb ./program
cat /proc/<PID>/maps
```

---

# 29. Final mental movie

```text
SOURCE CODE
    ↓
COMPILE + LINK
    ↓
EXECUTABLE PROGRAM
    ↓ run
KERNEL CREATES/MANAGES A PROCESS
    ↓
TEXT + DATA + HEAP + STACK
    ↓
SCHEDULER GIVES CPU TIME
    ↓
PC → FETCH → DECODE → EXECUTE
    ↓
FUNCTION CALL?
    ├── manage stack frame
    ├── preserve return information
    └── jump PC to callee
    ↓
DYNAMIC ALLOCATION?
    └── allocator manages heap block
    ↓
PROCESS KEEPS EVOLVING
    ↓
EXIT → kernel cleans process resources
```

> **Core idea:** a process is not just your source code. It is **machine instructions + memory regions + CPU execution state + kernel-managed metadata**.

---

## Repo files

```text
.
├── README.md
├── examples/
│   ├── process_demo.c
│   ├── stack_call_demo.c
│   ├── data_section_demo.c
│   └── heap_demo.c
└── imgs/
    ├── generated/
    │   ├── process-anatomy-map.svg
    │   ├── cpu-memory-hierarchy.svg
    │   ├── stack-heap-lifetime.svg
    │   └── function-call-flow.svg
    └── slides/
        ├── os_page-0026.png
        ├── ...
        └── os_page-0121.png
```

## Course visual source

- *Fundamentals of Operating Systems* — Anatomy of a Process section, slides **26–121**.
- Lecture transcript portions covering section introduction, program/process, simple execution, stack, detailed call tracing, data section, and Linux mapping demo.
