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

int main(void)
{
 pthread_t t0;
 pthread_t t1;

 signal(SIGTRAP, signal_handler);
 signal(SIGILL , signal_handler);
 signal(SIGHUP , signal_handler);

 pthread_create(&t0, NULL, worker, NULL);
 pthread_create(&t1, NULL, worker, NULL);

 pthread_join(t0, NULL);
 pthread_join(t1, NULL);

 exit(0);

}
