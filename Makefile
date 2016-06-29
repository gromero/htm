%.o: 	%.c
	gcc -c -g -O0 $< -o $@

%.S:	%.c
	gcc -S -g -O0 $< -o $@

all:	contention.o contention.S
	gcc -pthread -g -O0 -o contention contention.o

clean:
	rm -fr *.o contention
	rm -fr contention.S
