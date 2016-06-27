#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define THREAD_NUM 1

pthread_t thread_pool[THREAD_NUM];
static int lock = 0;

// char payload[THREAD_NUM] = { '>', '.', '+', '-' };

void htm_ok(void)
{
  printf("HTM OK. x->%d\n", lock);
}

void *thread_main_routine(void *arg)
{
 int i;
 pthread_t me;

 me = pthread_self();

 for (int j = 0; j < THREAD_NUM; j++)
 {

 }

 for (i = 0; i < THREAD_NUM; ++i)
  if (thread_pool[i] == me)
// printf("me: %d\n", i);


 // BODY
 while (1 == 1)
 {
loop01:
 sleep(2);
 asm goto (
           "xor 14,14,14       \n\t"
           "addis 14,14, %1@ha \n\t"
           "addi  14,14, %1@l  \n\t"
	   "mtctr 14           \n\t"
           "tbegin.            \n\t"
           "beqctr             \n\t" // tabort
           "ldx 14, 0, %0      \n\t"
           "addi 14, 14, 1     \n\t"
           "stdx 14, 0, %0     \n\t"
           "xor 14, 14, 14     \n\t"
//         "addis 14, 14, %2@ha \n\t"
//         "addi 14, 14, %2@l \n\t"
//         "mtctr 14 \n\t"
//         "bctrl   \n\t"
//         "bl htm_ok          \n\t" // <= This causes a tabort, since printf call write() syscall.
           "tend.              \n\t"
//         "xor 14, 14, 14     \n\t"
//         "addis 14, 14, %2@ha\n\t"
//         "addi  14, 14, %2@l \n\t"
//         "mtctr 14           \n\t"
//         "bctr               \n\t"
          :
          : "r"(&lock)
          : "r14"
          : htm_failed
	  );
        goto loop01;
htm_failed: printf("HTM failed\n");
// htm_ok:     printf("HTM OK. lock -> %d\n", lock);
//   __builtin_tbegin(0);
//   printf("%d\n", lock++);
//   __builtin_tend(0);
//   sleep(1);
 }
 return NULL;
}


int main(void)
{
 int x = 4;

 for (int i = 0; i < THREAD_NUM; ++i)
 {
   pthread_create(&thread_pool[i], NULL, thread_main_routine, NULL);
// printf("=%p\n", (void *) thread_pool[i]);
 }

 // Sit idle.
 while(1 == 1);
}
