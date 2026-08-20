# Demo Code

These two C files are **small reconstructions of the allocation experiment described in the lecture transcript**. They are not claimed to be the instructor's original source files.

- `alloc_many.c` — one pointer array + one `malloc()` per packet.
- `alloc_contiguous.c` — one large contiguous packet allocation.

The `Packet` layout is chosen to match the transcript's **44-byte packet** discussion using character arrays.

```bash
gcc -O2 -o alloc_many alloc_many.c
gcc -O2 -o alloc_contiguous alloc_contiguous.c

time ./alloc_many
time ./alloc_contiguous
```

Timings will vary by allocator, compiler, CPU, OS, and current system load.
