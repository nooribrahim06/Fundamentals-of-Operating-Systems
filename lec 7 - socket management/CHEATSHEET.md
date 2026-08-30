# Socket Management — 2-Minute Cheatsheet

## Server lifecycle

```text
socket() -> bind() -> listen() -> accept() -> read/recv() -> write/send() -> close()
```

## Two socket roles

```text
listen_fd  = accepts NEW TCP connections
client_fd  = communicates on ONE accepted TCP connection
```

## Connection establishment

```text
SYN -> SYN/ACK -> ACK
          |
          v
completed connection waits for accept()
```

## Per connected socket

```text
network -> receive buffer -> read() -> app
app -> write() -> send buffer -> network
```

## Never confuse

```text
request != connection != FD != thread != process
```

## Blocking problem

```text
read(fd_with_no_data) -> current thread waits
accept(empty_queue)   -> current thread waits
```

## Async families

```text
READINESS                       COMPLETION
"which FD can I use now?"       "do this operation and report done"
select / epoll / kqueue         io_uring / IOCP-style
```

## `select` vs `epoll`

```text
select: hand/scan a large FD set

epoll: register interest; receive ready events
```

## Node

```text
normal JS: one main event-loop thread
network I/O: OS async facilities via libuv
some operations: libuv thread pool
parallel JS: worker_threads / multiple processes
```

## Scaling listener designs

```text
fork()               -> multiple processes reference SAME listener
SO_REUSEPORT         -> multiple DISTINCT listeners on same IP:port
```

## Backend performance chain

```text
accept fast
  -> read fast
  -> avoid unnecessary blocking/copies
  -> keep CPU doing useful work
```
