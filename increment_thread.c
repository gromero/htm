/*
 * This code aims to be an example on:
 *
 * 1. Two level stringinification;
 * 2. How to use stringinified value in a comparison instruction resided a inline asm;
 * 2. How to call a function from inside a inline asm on PPC64.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_COUNTER 0x7FFF
#define MAX_THREAD  0x3

// Two levels of expansion to get a macro value stringinified.
#define STR(x) STR1(x)
#define STR1(x) #x

// Declaring counter variable as associated to a specific register is absolutely
// necessary here since we are using it in the comparison - cpwdi - inside inline
// asm. If it's not reclared this way it will reside in the locals, i.e. in the stack,
// and the inline comparison will just miss its updated value done by increment_counter()
// because in the inline asm we do not load counter value from the memory (locals, stack).
// This is tricky for sure. I'm using r15, but could be any volatile register.
//register int counter asm ("r15");

static int counter;
static int* counter_ptr = &counter;

pthread_t thread[MAX_THREAD];

void increment_counter(void)
{
  ++*counter_ptr;
}

void* thread_main_routine(void *arg)
{
//  while (counter < MAX_COUNTER)
//   increment_counter();
//  But in inline asm:

  asm(
     "           mflr 14                       \n\t"
     "increment: lwa 15, 0(%0)                 \n\t"
     "           cmpwi 15, " STR(MAX_COUNTER) "\n\t"
     "           bge exit                      \n\t"
     "           bl increment_counter          \n\t"
     "           b increment                   \n\t"
     "exit:      mtlr 14                       \n\t"
     : // no output
     : "r"(counter_ptr)
     : "r14","r15"
     );

  return NULL;
}


int main(void)
{
  counter = 0; // Just init the counter.

  for (int i = 0; i < MAX_THREAD; i++)
   pthread_create(&thread[i], NULL, &thread_main_routine,  NULL);

  for (int i =0; i < MAX_THREAD; i++)
   pthread_join(thread[i], NULL);

  printf("counter: %d\n", counter);
}
