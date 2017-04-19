#include <stdio.h>
#include <altivec.h>

int main(int argc, char **argv)
{


asm ("L: \n\t");
printf( "hello\n");
asm ("b L\n\t");


}
