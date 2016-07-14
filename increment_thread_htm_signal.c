/*
 * This code aims to be an example on:
 *
 * 1. Two level stringinification;
 * 2. How to use stringinified value in a comparison instruction resided a inline asm;
 * 2. How to call a function from inside a inline asm on PPC64;
 * 3. Verify the effect of a SIGTRAP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ucontext.h>

#include <asm/tm.h>
#include <signal.h>

#define MAX_COUNTER 0x7fff
#define MAX_THREAD  0x1

// Two levels of expansion to get a macro value stringinified.
#define STR(x) STR1(x)
#define STR1(x) #x

static int state[MAX_COUNTER];

static int counter;
static int* counter_ptr = &counter;

pthread_t thread[MAX_THREAD];

// Signal Handlers
void signal_handler(int signo)
{
  if (signo == SIGTRAP)
   printf("SIGTRAP\n");
  while (1 == 1);
}

void increment_counter(void)
{
  state[(*counter_ptr)++] = 1;
}

void failure_msg(void)
{
//  printf(".\n");
 unsigned long texasr = __builtin_get_texasr();
 unsigned int code = texasr >> (64 - 8);

/*
 TM_CAUSE_RESCHED       Thread was rescheduled.
 TM_CAUSE_TLBI          Software TLB invalid.
 TM_CAUSE_FAC_UNAV      FP/VEC/VSX unavailable trap.
 TM_CAUSE_SYSCALL       Syscall from active transaction.
 TM_CAUSE_SIGNAL        Signal delivered.
 TM_CAUSE_MISC          Currently unused.
 TM_CAUSE_ALIGNMENT     Alignment fault.
 TM_CAUSE_EMULATE       Emulation that touched memory.
*/

 switch (code)
 {
   case TM_CAUSE_RESCHED:  printf("Thread was rescheduled\n");
			   break;
   case TM_CAUSE_TLBI:     printf("Software TLB invalid\n");
	  	           break;
   case TM_CAUSE_FAC_UNAV: printf("FP/VEC/VSX unavailable trap\n");
	                   break;
   case TM_CAUSE_SYSCALL:  printf("Syscall from active transaction\n");
	                   break;
   case TM_CAUSE_SIGNAL:   printf("Signal delivered\n");
	                   break;
   case TM_CAUSE_MISC:     printf("Currently unused\n");
	                   break;
   case TM_CAUSE_ALIGNMENT:printf("Alignment fault\n");
	                   break;
   case TM_CAUSE_EMULATE:  printf("Emulation that touched memory\n");
	                   break;
   default:                ; // printf("0x%X: unknown error code!\n", code);
 }

/*
 TM_CAUSE_RESCHED       Thread was rescheduled.
 TM_CAUSE_TLBI          Software TLB invalid.
 TM_CAUSE_FAC_UNAV      FP/VEC/VSX unavailable trap.
 TM_CAUSE_SYSCALL       Syscall from active transaction.
 TM_CAUSE_SIGNAL        Signal delivered.
 TM_CAUSE_MISC          Currently unused.
 TM_CAUSE_ALIGNMENT     Alignment fault.
 TM_CAUSE_EMULATE       Emulation that touched memory.
*/

}

void* thread_main_routine(void *arg)
{
//  while (counter < MAX_COUNTER)
//   increment_counter();
//  + HTM (Hardware Transactional Memory) =

  asm(
     "           mflr 14                       \n\t"
     "           mr   16, %0                   \n\t" // Save counter_ptr in r16. Maybe use stack instead, since r15 is also volatile?
     "increment: tbegin.                       \n\t"
     "           beq failure                   \n\t"
     "           li 15, " STR(TM_CAUSE_SYSCALL) "\n\t"
     "           tabort. 15                    \n\t"
     "           b increment                   \n\t"
     "           lwa   15, 0(16)               \n\t" // Copy counter to r15.
     "           cmpwi 15, " STR(MAX_COUNTER) "\n\t"
     "           blt continue                  \n\t"
     "           xor   15, 15, 15              \n\t" // r15 = 0 .
     "           stw   15, 0(16)               \n\t" // counter = r15 = 0.
     "           b end                         \n\t" // b tend. ;-)
     "continue:  bl increment_counter          \n\t"
     "end:       tend.                         \n\t"
     "           b increment                   \n\t"
//   "failure:   mfspr 15, 130                 \n\t"
     "           li    17, 32                  \n\t"
     "           srd   15, 15, 17              \n\t"
     "           addis 17, 0 , 0xBE00          \n\t"
     "           addi  17, 17, 0x0001          \n\t"
     "           cmpw  15, 17                  \n\t"
     "failure:   bl failure_msg                \n\t"
     "           b increment                   \n\t"
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

  // Install a generic signal handler.
  if (signal(SIGTRAP, signal_handler) == SIG_ERR)
    exit(10);

  for (int i = 0; i < MAX_THREAD; i++)
   pthread_create(&thread[i], NULL, &thread_main_routine,  NULL);

  for (int i = 0; i < MAX_THREAD; i++)
   pthread_join(thread[i], NULL);

/*
  for (int i = 0; i < MAX_COUNTER; i++)
   printf("%d ", state[i]);
*/

  printf("counter: %d\n", counter);
}


/* References:

   [1] https://www.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.genprogc/transactional_memory.htm



 */
