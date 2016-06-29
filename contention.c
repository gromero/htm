#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define THREAD_NUM 1
#define STATE_NUM 2

#define STRINGINIFY_IT1(x) #x
#define STRINGINIFY_IT(x)  STRINGINIFY_IT1(x)

pthread_t thread_pool[THREAD_NUM];

static int state_pos = 0;
static int state[STATE_NUM];

void set_state_and_increment_pos(void)
{
 state[state_pos++] = 1;
}

void *thread_main_routine(void *arg)
{

update_state:
 asm goto (
           "xor   14,14,14    \n\t" // Load htm_failed label before entering the HTM.
           "addis 14,14,%3@ha \n\t" // Load label high word.
           "addi  14,14, %3@l \n\t" // Load label low word.
           "mtctr 14          \n\t" // Copy htm_failed address CTR, so we can jump to it in case of HTM failure.
           "tbegin.           \n\t" // HTM BEGIN.
           "beqctr            \n\t" // HTM FAILED.
           "bl set_state_and_increment_pos \n\t" // otherwise set current state to 1 and increment by 1 the current state position.
           "tend.             \n\t" // HTM END.
           "xor 14,14,14      \n\t" //
           "addis 14,14,%4@ha \n\t" //
           "addi  14,14,%4@l  \n\t" //
           "mtctr 14          \n\t" // Copy bail_out address to CTR, so we can jump to it in case state_pos >= STATE_NUM.
//         "cmpdi %0, " STRINGINIFY_IT(STATE_NUM) "\n\t"
//         "li 14, 4          \n\t"
           "cmpdi %0, 4       \n\t"
           "bgectr            \n\t" // bail_out if state_pos >= STATE_NUM.
           "xor   14,14,14    \n\t" // Load htm_succeeded label.
           "addis 14,14,%2@ha \n\t" //
           "addi  14,14,%2@l  \n\t" //
           "mtctr 14          \n\t" //
           "bctr              \n\t" // HTM SUCCEEDED.
          : // asm goto form does not allow use of input field.
          : "r"(state_pos), "r"(state)
          : "r14" // Scratch register. Volatile accordingly to the ABI.
          : htm_succeeded, htm_failed, bail_out // Labels used inside the inline asm.
          );


htm_succeeded:    printf("HTM succeeded\n");
	          goto update_state;

htm_failed:       printf("HTM failed\n");
                  goto update_state;

bail_out:         printf("Existing...\n");
	          while (1==1);
}


int main(void)
{
 // Initialize all states to zero.
 for (int i = 0; i < STATE_NUM; i++) state[i] = 0;

 // Spawn some threads that set current state as 1 and increment state_pos by 1.
 for (int i = 0; i < THREAD_NUM; i++)
   pthread_create(&thread_pool[i], NULL, thread_main_routine, NULL);

 // Wait all threads end.
 for (int i = 0; i < THREAD_NUM; i++)
  pthread_join(thread_pool[i], NULL);

 // Print all states.
 for (int i = 0; i < THREAD_NUM; i++)
 {
  if (! i % 200)
   printf("%d\n", state[i]);
  else
   printf("%d ", state[i]);
 }
}
