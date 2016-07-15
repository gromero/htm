#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <ucontext.h>
#include <pthread.h>
#include <signal.h>

void signal_handler(int sig)
{
 switch(sig)
 {
   case SIGTRAP: printf("Received a SIGTRAP\n");
                 while (1 == 1) ;
                 break;
   case SIGILL : printf("Received a SIGILL\n");
                 break;
   default     : printf("Received an unexpected signal!\n");
                 break;
 }

 printf("Exiting from signal_handler...\n");

}

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

 printf("I'm thread %lx. I'll trigger a hardware SIGTRAP in 4 seconds\n", my_id);

 // Wait a little bit to trigger SIGTRAP.
 sleep(4);
 asm ("trap     \n\t");

 while (1 == 1) ;
}

/* Main thread BLOCKs   SIGTRAP
   t0   thread BLOCKs   SIGTRAP
   t1   thread UNBLOCKs SIGTRAP
*/

int main(void)
{
 pthread_t t0;
 pthread_t t1;

 sigset_t sigset;

//  signal(SIGTRAP, signal_handler);
//  signal(SIGILL , signal_handler);
//  signal(SIGHUP , signal_handler);

 sigemptyset(&sigset);
 sigaddset(&sigset, SIGTRAP);

 pthread_sigmask(SIG_BLOCK, &sigset, NULL);
 pthread_create(&t0, NULL, worker_with_trap, (void*) &sigset);

 signal(SIGTRAP, signal_handler);
 signal(SIGILL , signal_handler);
 signal(SIGHUP , signal_handler);

 pthread_sigmask(SIG_BLOCK, &sigset, NULL);
 pthread_create(&t1, NULL, worker, (void*) &sigset);

 // Let's see if after setting mask for t1 main, which unblocks
 // SIGTRAP, main thread remains with SITRAP blocked.
 sigprocmask(SIG_UNBLOCK, &sigset, NULL);

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
