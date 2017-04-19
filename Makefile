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

threads02:	threads02.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads02.o -o threads02

threads03:	threads03.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads03.o -o threads03

threads04:	threads04.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads04.o -o threads04

threads05:	threads05.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads05.o -o threads05

threads06:	threads06.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads06.o -o threads06

threads07:	threads07.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads07.o -o threads07

threads08:	threads08.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads08.o -o threads08

threads09:	threads09.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads09.o -o threads09

threads10:	threads10.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 threads10.o -o threads10

zica:	zica.o
	gcc -O0 -g zica.c -o zica


clean:
	rm -fr *.o *.S contention
	rm -fr increment
	rm -fr increment_thread
	rm -fr increment_thread_htm
	rm -fr threads??
	rm -fr increment_thread_htm_signal
	rm -fr increment_thread_htm_syscall

