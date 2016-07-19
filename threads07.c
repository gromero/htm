#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <ucontext.h>
#include <pthread.h>
#include <signal.h>

#define SLEEP_TIME 4
#define STRING(x) XSTRING(x)
#define XSTRING(x) #x

/********************/
/*     HELPERS      */
/********************/

/*******************/
/* SIGNAL HANDLERS */
/*******************/

/* SIMPLE SIGNAL HANDLER */
void signal_handler(int sig)
{
 switch(sig)
 {
   case SIGTRAP: printf("Received a SIGTRAP\n");
                 // while (1 == 1) ;
                 break;
   case SIGILL : printf("Received a SIGILL\n");
                 break;
   default     : printf("Received an unexpected signal!\n");
                 break;
 }

 printf("Exiting from signal_handler...\n");
}


/* ADVANCED SIGNAL HANDLER */
void advanced_signal_handler(int signo, siginfo_t *si, void *data)
{
 ucontext_t *uc = (ucontext_t *)data; // Set a local pointer to uc.

 printf("* Received a SIGTRAP\n");
 printf("* si->si_addr = %p\n", si->si_addr);
 printf("* uc->uc_mcontext.regs->nip: %p\n", (void *) uc->uc_mcontext.regs->nip);

 // uc->uc_mcontext.regs->nip += 4; // Skip illegal instruction.
 uc->uc_mcontext.gp_regs[32] += 4; // Same as above ;-)
}

/***********/
/* WORKERS */
/***********/

void* worker(void *in)
{
 pthread_t my_id;

 my_id = pthread_self();

 while (1 == 1)
 {
   printf("I'm thread %lx\n", my_id);
   sleep(1);
 }
}

void* worker_with_trap(void *in)
{
 pthread_t my_id;

 my_id = pthread_self();

 while (1 == 1)
 {
  printf("I'm thread %lx. I'll trigger a hardware SIGTRAP every "
                                             STRING(SLEEP_TIME)
                                                   " seconds\n", my_id);

  // Wait a little bit to trigger SIGTRAP.
  sleep(SLEEP_TIME);

  // Unconditional trap instruction that provokes a HW exception.
  // It's a PowerPC specific instruction.
  asm (".long 0x0     \n\t");
 }
}

void* worker_with_htm(void *sig)
{
 pthread_t my_id;

 my_id = pthread_self();

 while (1 == 1)
 {
  printf("I'm thread %lx. I'll perform an HTM transaction every 1 seconde\n", my_id);
  sleep(1);
  asm (
       "1: tbegin. \n\t"
       "   beq 1b  \n\t"
       "   tend.   \n\t"
       "   b 1b    \n\t"
       :
       :
       :
      );
 }
}

/* Main thread BLOCKs   SIGTRAP -> idle worker
   t0   thread BLOCKs   SIGTRAP -> TRAP worker
   t1   thread UNBLOCKs SIGTRAP -> HTM  worker */

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
 sigaction(SIGILL , &sa, NULL);

 // Create the set containing signals to be unblocked.
 sigemptyset(&sigset);
 sigaddset(&sigset, SIGTRAP);
 sigaddset(&sigset, SIGILL );

 pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
 pthread_create(&t0, NULL, worker_with_trap, (void*) &sigset);

// signal(SIGTRAP, signal_handler);
// signal(SIGILL , signal_handler);
// signal(SIGHUP , signal_handler);

 pthread_sigmask(SIG_BLOCK, &sigset, NULL);
 pthread_create(&t1, NULL, worker /*_with_htm*/, (void*) &sigset);

 // Let's see if after setting mask for t1 main, which unblocks
 // SIGTRAP, main thread remains with SITRAP blocked.
 sigprocmask(SIG_BLOCK, &sigset, NULL);

 while (1 == 1)
 {
   printf("Hi, I'm the main thread ;-)\n");
   sleep(1); // MT-safe, hum?
 }

 // I don't expect pc will ever reach here.
 pthread_join(t0, NULL);
 pthread_join(t1, NULL);

 exit(0);
}
