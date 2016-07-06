/*
 * This code aims to be an example on:
 *
 * 1. Two level stringinification;
 * 2. How to use stringinified value in a comparison instruction resided a inline asm;
 * 2. How to call a function from inside a inline asm on PPC64.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_COUNTER 0x7fff  // In the end, this value must be printed exactly.
#define MAX_THREAD  0x45    // Keep this value less then the number of vCPU.

#define MAX_TIC     1000

// Two levels of expansion to get a macro value stringinified.
#define STR(x) STR1(x)
#define STR1(x) #x

static int state[MAX_COUNTER];

static int counter;
static int* counter_ptr = &counter;

static int tic = 0;

pthread_t thread[MAX_THREAD];
pthread_t thread_noise;

ucontext_t *ucp;
void increment_counter(void)
{
  state[(*counter_ptr)++] = 1;
}

void failure_msg(void)
{
  printf(".\n");
}

// We want to check if one thread doing a syscall
// periodic will increase the amount of HTM failures
// in other threads performing transactions.
// A dot (.) means HTM failure.
// A greater-then sign (>) means a "clock" tic,
// i.e. an introduced noise - sycall noise.
void* thread_noise_routine(void *arg)
{
  while (tic < MAX_TIC)
  {
    printf(">\n");
    tic++;
  }
}

void* thread_increment_routine(void *arg)
{
//  while (counter < MAX_COUNTER)
//   increment_counter();
//  + HTM (Hardware Transactional Memory) =

  asm(
     "           mflr 14                       \n\t"
     "           mr 16, %0                     \n\t" // Save counter_ptr in r16. Maybe use stack instead, since r15 is also volatile?
     "increment: tbegin.                       \n\t"
     "           beq failure                   \n\t"
     "           lwa 15, 0(16)                 \n\t" // Copy counter to r15.
     "           cmpwi 15, " STR(MAX_COUNTER) "\n\t"
     "           blt continue                  \n\t"
     "           li  17, 0xBE                  \n\t" // User-provided 8-bit tabort code. 0xBE means counter reached MAX_COUNTER.
     "	         tabort. 17                    \n\t"
     "continue:  bl increment_counter          \n\t"
     "           tend.                         \n\t"
     "           b increment                   \n\t"
     "failure:   mfspr 15, 130                 \n\t"
     "           li    17, 32                  \n\t"
     "           srd   15, 15, 17              \n\t"
     "           addis 17, 0 , 0xBE00          \n\t"
     "           addi  17, 17, 0x0001          \n\t"
     "           cmpw  15, 17                  \n\t"
     "           bnel failure_msg              \n\t"
     "           cmpw  15, 17                  \n\t" // Can it be removed?
     "           bne increment                 \n\t"
     "           mtlr 14                       \n\t"
     : // no output
     : "r"(counter_ptr)
     : "r14","r15", "r16", "r17"
     );

  return NULL;
}


int main(void)
{
  counter = 0; // Just init the counter.

  for (int i = 0; i < MAX_COUNTER; i++)
   state[i] = 0;

  pthread_create(&thread_noise, NULL, &thread_noise_routine, NULL);

  for (int i = 0; i < MAX_THREAD; i++)
   pthread_create(&thread[i], NULL, &thread_increment_routine,  NULL);

  // Wait all threads to finish before printing the counter.
  pthread_join(thread_noise, NULL);
  for (int i = 0; i < MAX_THREAD; i++) pthread_join(thread[i], NULL);

  printf("counter: %d\n", counter);
}


/* References:

   [1] https://www.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.genprogc/transactional_memory.htm

 */
