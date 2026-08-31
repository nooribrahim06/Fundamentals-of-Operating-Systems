# Optional Labs

These are intentionally small. The goal is to **see the concepts**, not become a compiler or Docker expert.

## Table of Contents

- [Lab 1 — See compilation and linking](#lab-1--see-compilation-and-linking)
- [Lab 2 — See user time vs kernel/system time](#lab-2--see-user-time-vs-kernelsystem-time)
- [Lab 3 — See a container as isolated processes](#lab-3--see-a-container-as-isolated-processes)

---

## Lab 1 — See Compilation and Linking

Create `math.c`:

```c
int add(int a, int b) {
    return a + b;
}
```

Create `main.c`:

```c
#include <stdio.h>

int add(int, int);

int main(void) {
    printf("%d\n", add(2, 3));
    return 0;
}
```

Compile separately:

```bash
gcc -c main.c -o main.o
gcc -c math.c -o math.o
```

Observe that object files exist but no `app` exists yet:

```bash
ls -lh main.o math.o
```

Link:

```bash
gcc main.o math.o -o app
./app
```

Inspect:

```bash
file app
readelf -h app
ldd app
```

**What you should see mentally:**

```text
.c → compiler → .o → linker → ELF executable
```

---

## Lab 2 — See User Time vs Kernel/System Time

Linux/macOS shell:

```bash
time cat /etc/hosts > /dev/null
```

On Linux, if `strace` is installed:

```bash
strace -c cat /etc/hosts > /dev/null
```

The point is not the exact numbers. Notice that a tiny user program relies on many kernel interfaces.

```text
cat
 ↓
open/read/write/close...
 ↓
kernel
```

---

## Lab 3 — See a Container as Isolated Processes

If Docker is installed:

```bash
docker run --rm -it --name os-lab alpine sh
```

Inside:

```sh
ps
cat /proc/1/status | head
hostname
ip addr
ip route
mount | head
```

From another host terminal:

```bash
docker top os-lab
docker stats os-lab
```

Ask yourself:

1. Why does the container see its own PID view?
2. Why does it have its own network interfaces/routes?
3. Is there another Linux kernel inside this Alpine container?
4. What mechanism would enforce a memory limit?

Try a resource limit:

```bash
docker run --rm --memory=128m alpine sh -c 'cat /proc/meminfo | head'
```

The conceptual target:

```text
Docker CLI/runtime
      ↓
namespaces + cgroups + rootfs/image layers
      ↓
normal processes on the host kernel
```
