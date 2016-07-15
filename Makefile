%.o: 	%.c
	gcc -c -g -O0 $< -o $@

%.S:	%.c
	gcc -S -g -O0 $< -o $@

all:	contention.o contention.S
	gcc -mhtm -mcpu=power8 -pthread -g -O0 -o contention contention.o

increment: increment.o increment.S
	gcc -mcpu=power8 -g -O0 -o increment increment.o

increment_thread: increment_thread.o increment_thread.S
	gcc -pthread -mcpu=power8 -g -O0 -o increment_thread increment_thread.o

increment_thread_htm: increment_thread_htm.o increment_thread_htm.S
	gcc -pthread -mhtm -mcpu=power8 -g -O0 -o increment_thread_htm increment_thread_htm.o

increment_thread_htm_syscall: increment_thread_htm_syscall.o increment_thread_htm_syscall.S
	gcc -pthread -mhtm -mcpu=power8 -g -O0 -o increment_thread_htm_syscall increment_thread_htm_syscall.o

increment_thread_htm_signal: increment_thread_htm_signal.o increment_thread_htm_signal.S
	gcc -pthread -mhtm -mcpu=power8 -g -O0 -o increment_thread_htm_signal increment_thread_htm_signal.o

threads01:	threads01.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads01.o -o threads01

clean:
	rm -fr *.o *.S contention
	rm -fr increment
	rm -fr increment_thread
	rm -fr increment_thread_htm
	rm -fr threads01
