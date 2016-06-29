%.o: 	%.c
	gcc -c -g -O0 $< -o $@

%.S:	%.c
	gcc -S -g -O0 $< -o $@

all:	contention.o contention.S
	gcc -mhtm -mcpu=power8 -pthread -g -O0 -o contention contention.o

clean:
	rm -fr *.o contention
	rm -fr contention.S
