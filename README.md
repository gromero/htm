- Example on how HTM on PPC64 works
- Use of user-provided tabort code as a failure hint to anything outside the atomic operation

Question: Can a SIGTRAP cause a HTM abortion? If so, what are the symptoms (IP, handler state)?

1. Can an additional thread performing just syscall disturb (abort) the HTM on the other threads?

   We'll try to elucidate this question by look at the abortion rate of HTMs in the presence of
   a noise thread performing periodic syscalls in comparison to aborting rates without a thread
   performing syscalls.

2. Can a signal cause the abortion of a HTM?

   We'll try to alucidate this by look at the abortion rate of HTMs in the presence of periodic
   SIGTRAP sent to the process where the threads performing HTM reside.


```
for i in `seq 1 200`; do ./increment_thread_htm |& fgrep .  |& wc -l |& tee -a increment_thread_htm_0x7FFF_0x3FFF; done
for i in `seq 1 100`; do ./increment_thread_htm_syscall |& gawk ' /\./ { htm_failures++ }  />/ { tics++ }  END {print "tics:",tics, "HTM failures:", htm_failures}'; done > ./increment_thread_htm_syscall_0x7FFF_15_100
for i in `seq 1 100`; do ./increment_thread_htm_syscall |& gawk ' /\./ { htm_failures++ }  />/ { tics++ }  END {print "tics:",tics, "HTM failures:", htm_failures}'; done | gawk '{i++;failures+=$5} END {print "Mean:", failures/i}'
```

### Backlog

- Measuring performance and contention at the same time is tricky, since optimum performance depends upon # of threads/ # of CPU threads;
- Measuring thread performance must be done indepedenytly of contention and firstly;
- Can't get TFIAR value after failure by any measn;
- Every thread has its own independent copy of TFIAR?;
- Does a signal interrupt all threads an cause an HTM abort a thread in such a state?;


### Conclusion

1. Since HTM block is atomic for any code outside, additional threads that perform syscalls outside the transaction have no effect on HTM failures (rate of abortion does not change);
2. The effect of contention is inconclusive if the test case spawns a total number of thread greater than the number of CPUs (HW threads?);
3. It's yet to be done the test to determine when a given number of threads causes performance degradation for the case test.
