%.o: 	%.c
	gcc -c -g -O0 $< -o $@

all:	contention.o
	gcc -pthread -g -O0 -o contention contention.o
