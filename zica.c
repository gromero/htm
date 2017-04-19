#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <stdint.h>
#include <inttypes.h>
#include <altivec.h>

int main(int argc, char **argv)
{
 register int x asm ("r4") = 12;
 register vector __int128  v0 asm ("vs32") = {0xBEEF};
asm (
     "xxlxor 32,32,32 \n\t"
     "xxlxor 33,33,33 \n\t"
     "xxlxor 34,34,34 \n\t"
     "xxlxor 35,35,35 \n\t"
     "xxlxor 36,36,36 \n\t"
     "xxlxor 37,37,37 \n\t"
     "xxlxor 38,38,38 \n\t"
     "xxlxor 39,39,39 \n\t"
     "xxlxor 40,40,40 \n\t"
     "xxlxor 41,41,41 \n\t"
     "xxlxor 42,42,42 \n\t"
     "xxlxor 43,43,43 \n\t"
     "xxlxor 44,44,44 \n\t"
     "xxlxor 45,45,45 \n\t"
     "xxlxor 46,46,46 \n\t"
     "xxlxor 47,47,47 \n\t"
     "xxlxor 48,48,48 \n\t"
     "xxlxor 49,49,49 \n\t"
     "xxlxor 50,50,50 \n\t"
     "xxlxor 51,51,51 \n\t"
     "xxlxor 52,52,52 \n\t"
     "xxlxor 53,53,53 \n\t"
     "xxlxor 54,54,54 \n\t"
     "xxlxor 55,55,55 \n\t"
     "xxlxor 56,56,56 \n\t"
     "xxlxor 57,57,57 \n\t"
     "xxlxor 58,58,58 \n\t"
     "xxlxor 59,59,59 \n\t"
     "xxlxor 60,60,60 \n\t"
     "xxlxor 61,61,61 \n\t"
     "xxlxor 62,62,62 \n\t"
     "xxlxor 63,63,63 \n\t"

     "xor    3, 3, 3  \n\t" // Clean r3
    
     "mtvrd  0, 3     \n\t" // vr0
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  1, 3     \n\t" // vr1
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  2, 3     \n\t" // vr2
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  3, 3     \n\t" // vr3
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  4, 3     \n\t" // vr4
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  5, 3     \n\t" // vr5
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  6, 3     \n\t" // vr6
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  7, 3     \n\t" // vr7
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  8, 3     \n\t" // vr8
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  9, 3     \n\t" // vr9
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  10, 3     \n\t" // vr10
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  11, 3     \n\t" // vr11
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  12, 3     \n\t" // vr12
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  13, 3     \n\t" // vr13
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  14, 3     \n\t" // vr14
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  15, 3     \n\t" // vr15
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  16, 3     \n\t" // vr16
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  17, 3     \n\t" // vr17
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  18, 3     \n\t" // vr18
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  19, 3     \n\t" // vr19
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  20, 3     \n\t" // vr20
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  21, 3     \n\t" // vr21
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  22, 3     \n\t" // vr22
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  23, 3     \n\t" // vr23
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  24, 3     \n\t" // vr24
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  25, 3     \n\t" // vr25
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  26, 3     \n\t" // vr26
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  27, 3     \n\t" // vr27
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  28, 3     \n\t" // vr28
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  29, 3     \n\t" // vr29
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  30, 3     \n\t" // vr30
     "addi   3, 3, 1  \n\t" // increment r3

     "mtvrd  31, 3     \n\t" // vr31
    );

}
