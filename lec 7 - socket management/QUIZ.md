# Active Recall — Socket Management

Do this without opening the README.

## Must Know

1. What is the difference between a listening socket and a connected socket?
2. What does `accept()` return?
3. Does `accept()` happen once per HTTP request or once per TCP connection?
4. Why can 10,000 clients all connect to the same server port?
5. What is the TCP 4-tuple?
6. Who performs SYN/SYN-ACK/ACK: your route handler or the kernel TCP stack?
7. What waits in the accept queue/state?
8. What is the receive buffer?
9. What is the send buffer?
10. What happens if your backend reads much slower than the client sends?
11. Why can a blocking `read()` starve other connections?
12. What is the difference between a connection FD and a thread?
13. What problem does `select()` solve?
14. Why does `select()` scale poorly compared with the `epoll` model?
15. What does `epoll_wait()` conceptually return?
16. Define readiness-based I/O in one sentence.
17. Define completion-based I/O in one sentence.
18. Why is regular file I/O awkward for pure socket-style readiness?
19. What are the submission and completion queues in the `io_uring` mental model?
20. In what sense is Node “single-threaded,” and in what sense is that statement incomplete?
21. What is the event loop?
22. What is the difference between fork-sharing a listener and `SO_REUSEPORT`?
23. Why can multiple acceptors on shared state need synchronization?
24. What does `sendfile()` try to avoid?
25. Why can compression/encryption defeat a simple zero-copy path?

## Final Explain-It Test

Explain this without notes:

```text
browser connects to :8000
        ↓
TCP handshake
        ↓
accept()
        ↓
connected FD
        ↓
request bytes arrive
        ↓
receive buffer
        ↓
backend reads/parses/processes
        ↓
write response
        ↓
send buffer
        ↓
network
```

If you can explain every arrow and say **which part is kernel vs user space**, you own this section.
