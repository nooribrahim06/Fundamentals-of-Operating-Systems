# Quick Recall — More OS Concepts

Use this **without opening `README.md` first**.

## Level 1 — Must Know

1. What is the CPU ultimately executing: source code, assembly text, bytecode, or machine code?
2. Why can an x86-64 executable not normally run natively on ARM64?
3. What does compilation produce before final linking?
4. What is the linker's job?
5. What is the difference between static and dynamic linking?
6. Name the common executable formats for Linux, Windows, and macOS.
7. Why does a runtime help portability?
8. What problem does JIT compilation try to solve?
9. What is the main trade-off of garbage collection?
10. What is the difference between user mode and kernel mode?
11. What controlled mechanism lets an application request kernel work?
12. Why can a page fault enter the kernel even though the application did not explicitly call a syscall?
13. Is a user→kernel mode switch automatically a process context switch?
14. Why can many tiny syscalls become expensive?
15. What is the defining kernel difference between a VM and a container?
16. What do namespaces provide?
17. What do cgroups provide?
18. What does a mount namespace isolate?
19. What does a network namespace isolate?
20. How can two containers both see a process with PID 1?
21. Why does OverlayFS help container storage efficiency?
22. Why can Linux containers run from Docker Desktop on Windows/macOS?

---

## Level 2 — Explain the Flow

### A. Native build

Fill in the blanks:

```text
source files
   ↓ _________
object files
   ↓ _________
executable
   ↓
OS loader
   ↓
process
```

### B. System call

Explain every arrow:

```text
backend code → read() → kernel mode → filesystem/device work → user mode
```

### C. Container

Explain why this is not a VM:

```text
Container A ─┐
Container B ─┼─> same Linux kernel → hardware
Container C ─┘
```

### D. Isolation

Match each mechanism:

```text
filesystem view   → ?
process IDs       → ?
network interfaces→ ?
CPU/RAM limits    → ?
```

---

## Answers

<details>
<summary>Open only after answering</summary>

1. Native machine instructions.
2. Different instruction-set architectures encode/define instructions differently.
3. Object files containing machine code plus metadata such as symbols/relocations/sections.
4. Resolve references and combine required objects/libraries into a final executable or linked artifact.
5. Static includes required library code in the binary; dynamic depends on separately loaded shared libraries.
6. ELF, PE, Mach-O.
7. The same application-level code can run on different platform-specific runtime implementations.
8. Avoid repeatedly interpreting hot code by compiling it to native machine code at runtime.
9. Easier memory management in exchange for runtime CPU/memory/synchronization/pause overhead.
10. User mode is restricted application execution; kernel mode has privileged access to system resources.
11. System call interface.
12. The CPU traps to the kernel when a memory mapping/permission situation needs OS handling.
13. No. The same thread can enter the kernel and return without scheduling another process.
14. Each crossing/dispatch/validation/state operation has fixed overhead.
15. A VM has a guest kernel; a container normally shares the host kernel.
16. Isolated views of kernel-managed resources.
17. Resource accounting/controls/limits.
18. Filesystem mounts/root view.
19. Network interfaces/routes/ports/network-stack view.
20. They are in different PID namespaces; the host kernel has a broader PID view.
21. Base image layers are shared read-only; each container stores only its writable changes.
22. Docker Desktop commonly runs Linux containers on a Linux kernel inside a small VM.

A. compiler → linker  
D. mount namespace; PID namespace; network namespace; cgroups.

</details>
