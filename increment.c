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

#define MAX_COUNTER 10

// Two levels of expansion to get a macro value stringinified.
#define STR(x) STR1(x)
#define STR1(x) #x

// Declaring counter variable as associated to a specific register is absolutely
// necessary here since we are using it in the comparison - cpwdi - inside inline
// asm. If it's not reclared this way it will reside in the locals, i.e. in the stack,
// and the inline comparison will just miss its updated value done by increment_counter()
// because in the inline asm we do not load counter value from the memory (locals, stack).
// This is tricky for sure. I'm using r15, but could be any volatile register.
register int counter asm ("r15");


void increment_counter(void)
{
  counter++;
}

int main(void)
{
  counter = 0; // Just init the counter.

  asm(
     "increment: cmpwi %0, " STR(MAX_COUNTER) "\n\t"
     "           bge exit                      \n\t"
     "           bl increment_counter          \n\t"
     "           b increment                   \n\t"
     "exit:      nop                           \n\t"
     : /* no output */
     : "r"(counter)
     : /* no clobber */
     );

 printf("counter: %d\n", counter);
}
