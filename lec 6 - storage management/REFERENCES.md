# Storage Management — References

These are **optional** deep dives. The README is designed to stand alone for revision.

## Course

- Hussein Nasser — *Fundamentals of Operating Systems*, Storage Management (slides 268–317).

## Related material supplied with the course

### Firefox and `fsync`

- Mozilla Bugzilla — **Firefox 3 uses fsync excessively**  
  https://bugzilla.mozilla.org/show_bug.cgi?id=421482

Why it matters: a concrete example of durability-related system calls producing major responsiveness/performance problems when used excessively.

### Linux and 4 KiB sectors

- IBM archived article — **Linux on 4KB Sector Disks**  
  https://web.archive.org/web/20190503044834/https://developer.ibm.com/tutorials/l-linux-on-4kb-sector-disks/

Why it matters: physical/logical sector sizes and alignment.

## Suggested topics for later deep dives

- NVMe command queues and DMA
- Linux block layer
- ext4 journaling
- XFS allocation and journaling
- SSD FTL implementation
- TRIM / discard
- database WAL + `fsync()`
- direct I/O and asynchronous storage I/O
