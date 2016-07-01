%.o: 	%.c
	gcc -c -g -O0 $< -o $@

%.S:	%.c
	gcc -S -g -O0 $< -o $@

all:	contention.o contention.S
	gcc -mhtm -mcpu=power8 -pthread -g -O0 -o contention contention.o

increment: increment.o increment.S
	gcc -mcpu=power8 -g -O0 -o increment increment.o

clean:
	rm -fr *.o *.S contention
