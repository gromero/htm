/*
 * How to abort with a user-provided code and get TEXASR
 * inspected for the code.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main(void)
{
  asm(
     "           tbegin.                       \n\t"
     "           beq exit                      \n\t"
     "           li 14, 0xAB                   \n\t" // Arbitrary user-provided code.
     "           tabort. 14                    \n\t"
     "           tend.                         \n\t"
     "exit:                                    \n\t"
     : // no output
     : "r"(counter_ptr)
     : "r14","r15", "r16", "r17"
     );

  uint64_t texasr = __builtin_get_texasr();
  uint8_t code = texasr >> 56; // Right way to inspect.

  printf("HTM failure code: 0x%X\n", code);
}


/*
   References:

   [1] http://lxr.free-electrons.com/source/tools/testing/selftests/powerpc/tm/tm-resched-dscr.c
   [2] https://github.com/torvalds/linux/blob/master/Documentation/powerpc/transactional_memory.txt#L174

*/
