#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <stdint.h>
#include <inttypes.h>
#include <altivec.h>

#define _ asm

int main(int argc, char **argv)
{
 register vector __int128 v0 asm ("vs32");
 register vector 
 void *p = malloc(16); // 128bit storage.

 v0 = {0xBEEF};

 _ ("tbegin.  \n\t");
 _ ("bne FAILURE \n\t");

 // Transactional code.
 v0 = {0xBABE};

 // Force all transactional code to abort.
 _ ("tabort.  \n\t");

 _ ("tend.    \n\t");
 _ ("b SUCCESS \n\t");

FAILURE:
        _ ("vcmpequb. 0, 0, 1 \n\t"); 


// salva vmx 1 na memoria
// traz o valor vsx 0 para vmx 1
// compara vmx 0 com vmx 1 (=vsx 0)
// restaura o valor vmx 1 da memoria
// 
// traz vsx 1 para vmx 0
// compara vmx 1 com vmx 0 
//
// traz vsx 2 para vmx 0
// compara vmx 2 com vmx 0
//
// ...
//
	printf("HTM failed\n");
        
	exit(1);

SUCCESS:
	printf("HTM succeeded\n");
	exit(0);
}
