# CPU Mini-Lab

Small commands only. The goal is to **observe** the lecture concepts, not benchmark professionally.

## 1. CPU Architecture + Core Topology

### Linux

```bash
uname -m
lscpu
nproc
```

Useful `lscpu` fields:

```text
CPU(s)
On-line CPU(s)
Thread(s) per core
Core(s) per socket
Socket(s)
NUMA node(s)
Model name
L1d/L1i/L2/L3 cache
```

### macOS — commands from the slides

```bash
sysctl -a | grep cachesize
sysctl -n hw.physicalcpu
uname -m
```

## 2. See Cache Topology on Linux

```bash
lscpu -C
```

If unsupported, inspect:

```bash
find /sys/devices/system/cpu/cpu0/cache -maxdepth 2 -type f -print
```

## 3. See NUMA Topology

```bash
lscpu | grep -i numa
```

If `numactl` is installed:

```bash
numactl --hardware
```

On a normal laptop you may see only one NUMA node — that is fine.

## 4. Watch CPU Time

```bash
top
```

Focus on:

```text
us sy id wa
```

Do **not** diagnose a production system from one snapshot.

## 5. One Busy CPU-Bound Process

Linux shell example:

```bash
yes > /dev/null
```

In another terminal:

```bash
top
```

Stop with `Ctrl+C`.

On a many-core machine, one `yes` process should roughly occupy one logical CPU, not the whole machine.

## 6. Multiple Busy Processes

Start several deliberately only for a short test:

```bash
yes > /dev/null &
yes > /dev/null &
yes > /dev/null &
yes > /dev/null &
```

Then:

```bash
top
```

Stop them:

```bash
killall yes
```

## 7. Observe PSI (Linux)

```bash
cat /proc/pressure/cpu
cat /proc/pressure/io
cat /proc/pressure/memory
```

Interpretation goal:

- CPU pressure → runnable work had to wait for CPU.
- I/O pressure → tasks stalled on I/O.
- Memory pressure → tasks stalled because of memory pressure/reclaim.

## Safety / Interpretation Notes

- `top` varies by OS/version.
- SMT makes “logical CPU count” larger than physical-core count on supporting CPUs.
- Cache/NUMA topology is hardware-specific.
- Synthetic commands like `yes` are teaching tools, not realistic backend benchmarks.
