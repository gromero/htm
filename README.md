### Study on Linux threads and signals

#### thread01.c

What happens when a multithread process get a signal?
- Does it stop all threads?
- Does it stop a single thread at random?

This simple code installs a signal handler. Since it is done before the thread
creation, t1 and t1 inherit the same signal mask. This means that t1 and t2 will
catch SIGTRAP, SIGILL, and SIGHUP. When a SIGTRAP arrives, the signal handler
will be lock in a idle loop. For SIGILL and SIGHUP the signal handler just print
a message saying it got the signal and goes away, return.

This example shows that you can lock all threads (main, t1 and t2) if you send
a SIGTRAP exactly three times, thus demonstrating the signal inheritance. Also,
it shows that interrupting a single thread (by sending just one SIGTRAP) won't
interrupt the remaining threads.

Finally, its show that sending SIGILL or SIGHUP signals will not lock any thread,
since the signal handler will return (will not stay forever in the idle loop),
and this can be verified by sending randomly as much as SIG{ILL,HUP} you wish
and verifying that you still need exactly three SIGTRAPs to lock all the
threads, given that you did not send any SIGTRAP before ;-)
