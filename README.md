### Study on Linux threads and signals

#### [threads01.c] (threads01.c)

What happens when a multithread process gets a signal?
- Does it stop all threads?
- Does it stop a single thread at random?

This simple code installs a signal handler. Since it is done before the thread
creation, t1 and t2 inherit the same signal mask. This means that t1 and t2 will
catch SIGTRAP, SIGILL, and SIGHUP. When a SIGTRAP arrives, the signal handler
will be lock in a idle loop. For SIGILL and SIGHUP the signal handler just print
a message saying it got the signal and goes away, return.

This example shows that you can lock all threads (main, t1 and t2) if you send
a SIGTRAP exactly three times, thus demonstrating the signal inheritance. Also,
it shows that interrupting a single thread (by sending just one SIGTRAP) won't
interrupt the remaining threads.

Finally, it shows that sending SIGILL or SIGHUP signals will not lock any thread,
since the signal handler will return (will not stay forever in the idle loop),
and this can be verified by sending randomly as much as SIG{ILL,HUP} you wish
and verifying that you still need exactly three SIGTRAPs to lock all the
threads, given that you did not send any SIGTRAP before ;-)

#### [threads02.c] (threads02.c) vs [threads03.c] (threads03.c)

This is a example on how to block or unblock a set of signals as per thread. At
first, it seems straightforward that just passing `&setsig` and the action on it,
i.e `SIG_BLOCK or SIG_UNBLOCK` when creating a thread is just fine - when calling
`pthread_create()`, however, in fact, the last call on this function will also
determine the block state in the main thread. So to adjust the main thread block
state it's necessary to call `sigprocmask()` *after* we create and set the block
state for the last thread (in this case *t1*). Thus in [threads02.c] (threads02.c)
main thread is not really blocked regarding SIGTRAP since `sigprocmask()` it's
called before `pthread_create()`. However, in [threads03.c] (threads03.c) SIGTRAP
is blocked, since `sigprocmask()` is called after `pthread_create()`.
