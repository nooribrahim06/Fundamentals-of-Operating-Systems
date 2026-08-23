# Active Recall Quiz

Do these **without opening the notes**.

## Level 1 — Must Know

1. What is the difference between ALU, control logic, MMU, and registers?
2. Order these from closest/faster to farther/slower: RAM, L1, registers, L3, L2.
3. What is the difference between L1I and L1D?
4. What is a cache line?
5. Why can two cores caching `A=1` become a correctness problem?
6. Who performs cache-coherence communication: your app, the OS, or CPU hardware?
7. Define NUMA in one sentence.
8. What is the difference between a physical core and an SMT logical CPU?
9. What does a TLB cache?
10. Why can context switching between address spaces affect the TLB?

## Level 2 — Instruction Execution

11. Walk through `SUB SP, SP, 12` from PC to writeback.
12. Why can the first instruction fetch miss but the next several instructions hit L1I?
13. Why can a function call hurt instruction-cache locality?
14. What is the tradeoff of inlining?
15. Why is “64-bit CPU means PC += 8” wrong?

## Level 3 — Do Not Mix These

16. Explain pipelining using one physical core.
17. Explain parallelism using multiple physical cores.
18. Explain SMT using one physical core.
19. Explain SIMD using one instruction.
20. Why does branch prediction help a pipeline?
21. What is the high-level Spectre lesson about speculative execution?

## Level 4 — Backend Bridge

22. Why can one busy single-threaded process show ~25% aggregate CPU on a 4-core machine?
23. Why can 100% CPU utilization be okay?
24. What is the difference between utilization and saturation?
25. What does `top` `us` mean?
26. What does `sy` mean?
27. What does `id` mean?
28. What does `wa` mean, and why is “CPU blocked waiting” an oversimplification?
29. Why doesn't normal network socket waiting necessarily show as high `wa`?
30. Give one CPU-bound and one I/O-bound backend workload.
31. Why does an I/O-bound workload naturally lead us toward concurrency?
32. Why does a CPU-bound workload lead us toward parallelism?
33. How can NUMA interact with thread scheduling?
34. How can cache coherence become a multithreading performance problem?

## One Final Whiteboard

Draw from memory:

```text
Application work
   ↓
process/thread
   ↓
scheduler
   ↓
logical CPU
   ↓
physical core
   ↓
pipeline / execution units
   ↓
registers + caches
   ↓
RAM
```

Then add where **SMT**, **NUMA**, **TLB**, and **I/O wait** fit.
