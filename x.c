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
 vector __int128 vmx_scratch_area[2];

 vmx_correct_value[0] = (vector __int128) {0xC0DE};
 vmx_correct_value[1] = (vector __int128) {0xC1DE};

 vmx0 = vmx_correct_value[0]; 

 vmx0 = (vector __int128) {0xBEEF};
 //vmx0 = (vector __int128) {0xC0DE};
 vmx1 = vmx0;

 _ ("tbegin.  \n\t");
 _ goto ("beq %l[_failure] \n\t" : : : : _failure);

 // Transactional code.
 // vmx0 = (vector __int128) {0xBABE};

 // Force all transactional code to abort.
 _ ("tabort. 0 \n\t");

 _ ("tend.    \n\t");

 _ goto ("b %l[_success] \n\t" : : : : _success);

_failure:
        _      ("stvx 1, 0, %[vmx_scratch_area]  \n\t" : : [vmx_scratch_area]  "r"(vmx_scratch_area)  : "memory");	
        _      ("lvx  1, 0, %[vmx_correct_value] \n\t" : : [vmx_correct_value] "r"(vmx_correct_value) :         ); 
        _      ("vcmpequb. 0, 0, 1               \n\t");
	_ goto ("bc 12, 24, %l[_value_match]      \n\t" : : : : _value_match);
	  goto _value_mismatch;

_value_mismatch:
        printf("HTM failed and VMX registers got corrupted!\n");
	exit(13);


// salva vmx 1 na memoria
// traz o valor correto para vmx 1
// compara vmx 0 com vmx 1 (valor correto)
// restaura o valor vmx 1 da memoria
// 
// traz valor correto de vmx 1 para vmx 0
// compara vmx 1 com vmx 0 
//
// traz valor correto de vmx 2 para vmx 0
// compara vmx 2 com vmx 0
//
// ...
//
_value_match:
	printf("HTM failed but VMX registers are OK\n");
	exit(1);

_success:
	printf("HTM succeeded\n");
	exit(0);
}
