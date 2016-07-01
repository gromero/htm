#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define THREAD_NUM 1
#define STATE_NUM  4

#define HTM_ABORT_DONE    0xBE
#define HTM_ABORT_FAILURE 0x00


#define STRINGINIFY_IT1(x) #x
#define STRINGINIFY_IT(x)  STRINGINIFY_IT1(x)

pthread_t thread_pool[THREAD_NUM];

register int state_pos asm("r16");
int state[STATE_NUM];

void set_state_and_increment_pos(void)
{
//  state[state_pos++] = 1;
 state_pos++;
}

// Used just to debug.
unsigned long get_texasr(void)
{
 return __builtin_get_texasr();
}

void *thread_main_routine(void *arg)
{

 asm (
           "begin:   tbegin.                        \n\t" // HTM BEGIN.
           "         beq  failure                   \n\t" // HTM FAILED.
           "         cmpwi %0, " STRINGINIFY_IT(STATE_NUM) "\n\t"
           "         blt continue                    \n\t"
           "         li    14, 0xBE                \n\t"
           "         tabort. 14                    \n\t" // state_pos = STATE_NUM
           "continue: bl set_state_and_increment_pos \n\t" // otherwise set current state to 1 and increment by 1 the current state position.
           "         tend.                          \n\t" // HTM END.
           "         b begin                        \n\t"
          "failure: addi 14, 0, 0xBE               \n\t"
          "         mfspr 15, 130                  \n\t"
           "         sradi 15, 15, 32               \n\t"  // Shift Right Algebraic Double Word Immediate.
           "         addis 14, 0, 0xBE00            \n\t"
           "         addi  14, 14, 0x0001           \n\t"
           "         cmpd  15, 14                   \n\t"
           "         bne begin                     \n\t"
          :
          : "r"(state_pos)
          : "r14", "r15" // Scratch registers.
          );
}


int main(void)
{
 // Init state_pos;
 state_pos = 0;

 // Initialize all states to zero.
 for (int i = 0; i < STATE_NUM; i++) state[i] = 0;

 // Spawn some threads that set current state as 1 and increment state_pos by 1.
 for (int i = 0; i < THREAD_NUM; i++)
   pthread_create(&thread_pool[i], NULL, thread_main_routine, NULL);


 // Wait all threads end.
 for (int i = 0; i < THREAD_NUM; i++)
  pthread_join(thread_pool[i], NULL);

 // Print all states.
 for (int i = 0; i < STATE_NUM; i++)
 {
  if (! i % 200 && i != 0)
   printf("%d\n", state[i]);
  else
   printf("%d ", state[i]);
 }
 printf("\n");

 printf("state_pos -> %d\n", state_pos);
}
