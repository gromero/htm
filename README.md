- Example on how HTM on PPC64 can be in an atomic operation
- Use of user-provided tabort code as a failure hint to anything outside the atomic operation

Question: Can a SIGTRAP cause a HTM abortion? If so, what are the symptoms (IP, handler state)?

1. Can an additional thread performing just syscall disturb (abort) the HTM on the other threads?

   We'll try to ellucidate this question by look at the abortion rate of HTMs in the presence of
   a noise thread performing periodic syscalls in comparison to aborting rates without a thread
   performing syscalls.

2. Can a signal cause the abortion of a HTM?

   We'll try to alucidate this by look at the abortion rate of HTMs in the presence of periodic
   SIGTRAP sent to the process where the threads performing HTM reside.
