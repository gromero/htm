#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_COUNTER 10

#define STR(x) STR1(x)
#define STR1(x) #x

int counter;

void increment_counter(void)
{
 counter++;
}


int main(void)
{

 asm(
    "count: cmpwi %0, " STR(MAX_COUNTER) "\n\t"
    "bge exit \n\t"
    "bl increment_counter \n\t"
    "b count \n\t"
    "exit:  nop \n\t"
    : "=r"(counter)
    : "0"(counter)
    : "r14"
    );

 printf("counter: %d\n", counter);

}
