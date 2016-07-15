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
i.e `SIG_BLOCK or SIG_UNBLOCK`, when creating a thread is just fine - when calling
`pthread_create()`. However, in fact, the last call on this function will also
determine the block state in the main thread. So to adjust the main thread block
state it's necessary to call `sigprocmask()` *after* we create and set the block
state for the last thread (in this case *t1*). Thus in [threads02.c] (threads02.c)
main thread is not really blocked regarding SIGTRAP since `sigprocmask()` it's
called before `pthread_create()`. However, in [threads03.c] (threads03.c) SIGTRAP
is blocked, since `sigprocmask()` is called after `pthread_create()`.

#### [threads04.c] (threads04.c)

This example tries to show two things: (1) that signals can be delivered as per
thread and (2) a SIGTRAP caught from a `kill -SIGTRAP <pid>` is not the same as
a SIGTRAP that is due to a `trap` instruction, i.e. in the case it is due to a
hardware exception. This is really expected since "In the case of a hardware ex-
ception blocking the signal makes little sense as it is unclear how a program
should then continue execution. [Thus] starting with Linux 2.6, if the signal is
blocked, then the process is always immediately _killed by the signal_, even if
the process has installed a handler for the signal" [1].

Hence this example exactly tries to show this behavior by setting the main thread
to accept a SIGTRAP but setting the thread *t1* (which genereates a hardware trap)
as to deny a SIGTRAP. Thus when a SIGTRAP from `kill` arrives it hits the handler
of main thread (kernel randomly picks the thread that unblocks this signal, in
that case just the main thread), but since *t1* blocks SIGTRAP and also executes
an `trap` instruction - and SIGTRAP from hardware cannot be denied - the process
as whole is killed by the kernel:

```
$ ./threads04
Hi, I'm the main thread ;-)   <== main thread
I'm thread 3fff809af1a0. I'll trigger a hardware SIGTRAP in 4 seconds <== t1
I'm thread 3fff801af1a0 <== t0
I'm thread 3fff801af1a0
Hi, I'm the main thread ;-)
I'm thread 3fff801af1a0
Hi, I'm the main thread ;-)
I'm thread 3fff801af1a0
Hi, I'm the main thread ;-)
Trace/breakpoint trap
```
and if a SIGTRAP is sent by `kill` command the main thread accepts it, nonetheless
when the SIGTRAP due to a HW exception arrives thread *t1* the whole process dies:

```
$ ./threads04
Hi, I'm the main thread ;-) <== main thread
I'm thread 3fff9acaf1a0. I'll trigger a hardware SIGTRAP in 4 seconds <== t1 thread
I'm thread 3fff9a4af1a0 <== t0 thread
Hi, I'm the main thread ;-)
I'm thread 3fff9a4af1a0
Received a SIGTRAP <== kill -SIGTRAP, main thread gets it fine and stops
I'm thread 3fff9a4af1a0 <== just t0 continues to present itself, t1 is sleeping
I'm thread 3fff9a4af1a0 <== and main thread is in an idle loop after the SIGTRAP
Trace/breakpoint trap <== but then t1 executes the trap HW instruction
```



[1] Section 22.4 - Hardware-Generated Signals, in *The Linux Programming Interface*,
Michael Kerrisk.
