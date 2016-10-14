HTM investigations
==================

### Introduction

- Example on how HTM on PPC64 LE is an atomic operation;
- Usage of user-provided tabort code as a failure hint to anything outside the HTM block.

Question: Can a SIGTRAP cause a HTM abortion? If so, what are the symptoms (IP, handler state)?

1. Can an additional thread performing just syscall disturb (abort) the HTM on the other threads?

   We'll try to elucidate this question by look at the abortion rate of HTMs in the presence of
   a noise thread performing periodic syscalls in comparison to aborting rates without a thread
   performing syscalls.

2. Can a signal cause the abortion of a HTM?

   We'll try to alucidate this by look at the abortion rate of HTMs in the presence of periodic
   SIGTRAP sent to the process where the threads performing HTM reside.


- Some idea about how to measure HTM abortion rate:

```
for i in `seq 1 200`; do ./increment_thread_htm |& fgrep .  |& wc -l |& tee -a increment_thread_htm_0x7FFF_0x3FFF; done
for i in `seq 1 100`; do ./increment_thread_htm_syscall |& gawk ' /\./ { htm_failures++ }  />/ { tics++ }  END {print "tics:",tics, "HTM failures:", htm_failures}'; done > ./increment_thread_htm_syscall_0x7FFF_15_100
for i in `seq 1 100`; do ./increment_thread_htm_syscall |& gawk ' /\./ { htm_failures++ }  />/ { tics++ }  END {print "tics:",tics, "HTM failures:", htm_failures}'; done | gawk '{i++;failures+=$5} END {print "Mean:", failures/i}'
```

### Backlog

- Measuring performance and contention at the same time is tricky, since optimum performance depends upon # of threads/ # of CPU threads;
- Measuring thread performance must be done indepedenytly of contention and firstly;

### Conclusion

1. Since HTM block is atomic for any code outside, additional threads that perform syscalls outside the transaction have no effect on HTM failures (rate of abortion does not change);
2. The effect of contention is inconclusive if the test case spawns a total number of thread greater than the number of CPUs (HW threads?);
3. It's yet to be done the test to determine when a given number of threads causes performance degradation for the case test.


A minuscule study on Linux threads, signals, and HTM
====================================================================

> First of all, __before you get insane__, let me say that POWER ISA, any version,
> uses the so-called IBM bit numbering scheme (aka [MSB 0] (https://goo.gl/AjTpCQ)).
> So whenever you read the MSR (Machine State Register) bit offsets (for
> instance), please, mind that you have to read as 63-BIT_OFFSET (regardless of the
> endianess, since shifts are performed as per value in register).
> Hence, for example, the MSR TS (Machine State Register Transaction Bits) are
> said to be bits 29:30, on subsection 3.2.1, [POWER ISA V2.07B, POWER8] (https://goo.gl/jrvlZS),
> __however, actually,__ for mask and shift purposes, you want to think them as
> bits 63-29:63-30, or 34:33, that's the reason you gonna see them in Linux kernel
> source code as 34:33, and not as 29:30: [arch/powerpc/include/asm/reg.h] (https://goo.gl/YmYUcV).
> This by no means it reflects the endianess, being just a matter of notation.
> Even if in general Little-endian archs employ LSB 0 whilst BE archs employ MSB 0,
> it could be the opposite, since LSB 0 and MSB 0 are just two different
> convenient ways to refer to bits in a register.

#

> `printf()` is said to be a _persona non grata_ inside a signal handler. As of
> 2015/08 its manual says "MT-Safe", but there is "locale" after, so not sure.
> Anyway it's not that important for our purpose here, AFAICS.

### [threads01.c] (threads01.c)

What happens when a multithread process gets a signal:
- Does it stop all the threads in a process?
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

### [threads02.c] (threads02.c) vs [threads03.c] (threads03.c)

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

### [threads04.c] (threads04.c)

This example tries to show two things: (1) that signals can be delivered as per
thread and (2) a SIGTRAP caught from a `kill -SIGTRAP <pid>` is not the same as
a SIGTRAP that is due to a `trap` instruction, i.e. in the case it is due to a
hardware exception. This is really expected since "In the case of a hardware ex-
ception blocking the signal makes little sense as it is unclear how a program
should then continue execution. [Thus] starting with Linux 2.6, if the signal is
blocked, then the process is **always** immediately **killed by the signal**,
even if the process has installed a handler for the signal" [1].

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

### [threads05.c] (threads05.c), [threads06.c] (threads06.c)

Now things start to become interesting. An executiong of trap/illegal instruction
inside a thread (could also be in a single process) yields an infite loop. Signal
handler returns but get again the instruction.

It's necessary to know the context were the trap/illegal instruction happened to
then know the pc and manage it, I mean, increment the pc to point at the instruc-
tion after the trap/illegal instruction.

[threads05.c] (threads05.c) examplifies that case with a single handler whilst
[threads06.c] (threads06.c) examplifies that case with an advanced handler.

If it was on a x64 we could do that, given that `uc` is `ucontext_t *uc`, we can
do this: `uc->uc_mcontext.greps[REG_RIP] += __offender_instruction_length__`.

But I've got good news: POWER8 lacks an IP (Instruction Pointer) register. So
what now? Well, the Linux kernel kindly takes care of it ;-) It means that the
next instruction pointer regarding the context interrupted async by the signal is
available thought the member `uc->uc_mcontext.regs->nip`.

Nonetheless, we'll try this approach on the next example. By now this example just
demonstrates that infinite loop behavior

### [threads07.c] (threads07.c)

This example solves infite loop by adding 4 bytes in the nip (next instruction
pointer). On PPC64 LE all instructions have the same length, 4 bytes. So skipping
the offending struction is trivial.

### [threads08.c] (threads08.c)

__Now things start to get interesting__. One thread is set to perform a HTM with a
trap instruction inside. An advanced signal hander is set to get the signal.

HTM fails due to a `trap` instruction inside a HTM block delimitted by `tbegin.`
and `tend.`. It's possible to verify that, although `si->si_addr` contains not
them same value as `uc->uc_mcontext.regs->nip`,  what would be the normal case,
`uc->uc_link` isn't, on the other hand, NULL. Thus in fact we have a second `ucontext_t`
struct that is related exactly to the context where the `trap` instruction inside
the HTM block, the instruction responsible for the generation of the SIGTRAP) was
executed. So indeed we have two ucontext_t in this case: (a) inside HTM, where
pc = tbegin. + 4 and (b) where a `trap` instruction was executed - in this
example it's inside the HTM, just after the `beq` - the HTM failure handler - but
could be anywhere else.

### [threads09.c] (threads09.c)

This test case demonstrates that given a number of nested HTM blocks, if any HTM
transaction fails, then the first context (uc) will have a (nip) point to the
most outer HTM failure handler. si->si_addr, nonetheless, still points fine to
the primary cause of HTM failure, i.e to the trap instruction inside the most
inner HTM transaction.

### [threads10.c] (threads10.c)

A very simple example showing that the checking for a signal caught in
transaction suggested in the Linux kernel documentation is just fine both on
PPC64 BE and LE. If it was not the case, the detection of a signal __NOT__ caught
in transaction would at some point be recognized as signal caught in transaction.
As a consequence, once returning from the signal handler it would not have the
context's nip incremented to point one instruction after the `trap` instruction,
looping forever, as pointed out already commented in the example [threads05.c] (threads05.c)
and [threads06.c] (threads06.c).

### TODO

 * Exception vs interruption (explain conceptual differences);
 * HW exception/interruption vs SW exception/interruption;
 * Does 'INT' on x64 is like a simple jump or it relies on a sort of table? On Linux
   kernel, is the ISR that takes care of inspecting a table to find which address to
   jump or its a burden of INT mechanism. (I'm assuming that it's just a jump and that
   the Linux kernel installs the ISR);
 * What is the ultimate cause of a process being able to ignore SIGTRAP from `kill`
   but not from a `trap` instruction;
 * Explain why a signal in HTM is quite different from a syscall in HTM;
 * It is said that an TM on x64 has no time out (you can spend as much time as
   you want inside an atomic block). Verify that and also check if it behaves the
   same on Power.
