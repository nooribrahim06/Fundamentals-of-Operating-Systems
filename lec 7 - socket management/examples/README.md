# C Demo

This demo is intentionally blocking and single-client-at-a-time.

## Build

```bash
gcc -Wall -Wextra -O0 -g c-web-server.c -o c-web-server
```

## Run

```bash
./c-web-server
```

## Test from another terminal

```bash
curl -v http://127.0.0.1:8080/
```

## Inspect the listener on Linux

```bash
ss -ltnp | grep 8080
```

## What to watch

1. Before `curl`, the program is waiting in blocking `accept()`.
2. `curl` creates a TCP connection; `accept()` returns a new `client_fd`.
3. `read(client_fd, ...)` copies request bytes into the process buffer.
4. `write(client_fd, ...)` hands response bytes to the kernel socket path.
5. `close(client_fd)` closes this demo connection.
6. The loop returns to `accept()` for the next connection.

## Why this is not production-ready

- blocking one-client-at-a-time architecture,
- tiny/simple HTTP parser (actually no real parser),
- closes every connection,
- no TLS,
- no partial-request protocol handling,
- no timeouts or resource limits,
- no async/event loop/thread pool.

That is intentional: the goal is to expose the OS primitives clearly.
