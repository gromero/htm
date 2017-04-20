#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <altivec.h>

#define _ asm

int main(int argc, char **argv)
{
 register vector __int128 vsx0 asm ("vs0");
 register vector __int128 vsx1 asm ("vs1");

 register vector __int128 vmx0 asm ("vs32");
 register vector __int128 vmx1 asm ("vs33");

 vector __int128 vmx_correct_value[32];

 vmx_correct_value[0] = (vector __int128) {0xC0DE};
 vmx_correct_value[1] = (vector __int128) {0xC1DE};

 vmx0 = vmx_correct_value[0]; 

 vmx0 = (vector __int128) {0xBEEF};

 _ ("tbegin.  \n\t");
 _ goto ("beq %l[_failure] \n\t" : : : : _failure);

 // Transactional code.
 vmx0 = (vector __int128) {0xBABE};

 // Force all transactional code to abort.
 _ ("tabort. 0 \n\t");

 _ ("tend.    \n\t");

 _ goto ("b %l[_success] \n\t" : : : : _success);

_failure:
        _      ("lvx 2, 0, %0 \n\t" : : "r"(vmx_correct_value) : "r5"); 
        _      ("vcmpequb. 0, 0, 2 \n\t");
	_ goto ("bne 6, %l[_bail_out] \n\t" : : : : _bail_out);
        _ goto ("b %l[_bail_out2] \n\t"     : : : : _bail_out2);
_bail_out:
 exit(13);


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
_bail_out2:
	printf("HTM failed\n");
	exit(1);

_success:
	printf("HTM succeeded\n");
	exit(0);
}
