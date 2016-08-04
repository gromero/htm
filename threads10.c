#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>
#include <pthread.h>
#include <signal.h>

void advanced_signal_handler(int signo, siginfo_t *si, void *data)
{
 ucontext_t* uc = (ucontext_t *) data; // Set a local pointer to uc.
 ucontext_t* t_uc = uc->uc_link;       // Second context HTM.

 if (t_uc != NULL) {
  if (t_uc->uc_mcontext.regs->msr & 0x600000000) {
    printf("* Caught a SIGTRAP in transaction\n");
    printf("* si->si_addr = %p\n", si->si_addr);
    printf("* uc->uc_mcontext.regs->nip: = %p\n", (void *) uc->uc_mcontext.regs->nip);
    printf("* t_uc->uc_mcontext.regs->nip = %p\n", (void *) t_uc->uc_mcontext.regs->nip );
    return;
  }
 }
    printf("* Caught a SIGTRAP but NOT in transaction\n");
    printf("* si->si_addr = %p\n", si->si_addr);
    printf("* uc->uc_mcontext.regs->nip = %p\n", (void *) uc->uc_mcontext.regs->nip);
    uc->uc_mcontext.regs->nip += 4;
}

void* worker_with_just_a_trap(void *sig)
{
 pthread_t my_id;

 my_id = pthread_self();

 while (1 == 1)
 {
  printf("=> I'm thread %lx. I'll perform JUST A TRAP every 2 seconds\n", my_id);
  sleep(2);
  asm (
       "   trap           \n\t"
      );
 }
}

void* worker_with_htm_and_trap(void *sig)
{
 pthread_t my_id;

 my_id = pthread_self();

 while (1 == 1)
 {
  printf("=> I'm thread %lx. I'll perform A TRAP IN TRANSACTION every 1 second\n", my_id);
  sleep(1);
  asm (
       "   tbegin.        \n\t" // Begin HTM
       "   beq  2f        \n\t" // HTM abort handler.
       "   trap           \n\t"
       "   mr  15, 16     \n\t" // Do something (I'll never reach here).
       "   mr  14, 15     \n\t" // Do something (I'll never reach here).
       "   tend.          \n\t" // End HTM.
       "2:                \n\t"
       : /* no output */
       : /* no input  */
       : "r14", "r15", "r16"
      );
 }
}

int main(void)
{
 pthread_t t0;
 pthread_t t1;

 sigset_t sigset;

 struct sigaction sa;

 // Install thread-shared signal handler.
 // sa.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;
 sa.sa_flags = SA_SIGINFO;
 sa.sa_sigaction = advanced_signal_handler;
 sigaction(SIGTRAP, &sa, NULL);

 // Create the set containing signals to be unblocked.
 sigemptyset(&sigset);
 sigaddset(&sigset, SIGTRAP);

 // Create thread t0
 pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
 pthread_create(&t0, NULL, worker_with_just_a_trap, (void*) &sigset);

 // Create thread t1
 pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
 pthread_create(&t1, NULL, worker_with_htm_and_trap, (void*) &sigset);

 // SIGTRAP, main thread remains with SIGTRAP blocked.
 sigprocmask(SIG_BLOCK, &sigset, NULL);

 while (1 == 1)
 {
   sleep(1); // MT-safe, hum?
 }

 // I don't expect pc will ever reach here. Thread workers are looping forever.
 pthread_join(t0, NULL);
 pthread_join(t1, NULL);

 exit(0);
}
