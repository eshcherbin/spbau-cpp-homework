all: main.o bmp.o
	gcc main.o bmp.o -o main

main.o: main.c bmp.h
	gcc -c main.c -Wall -Wextra -Wshadow -std=c11

bmp.o: bmp.c bmp.h
	gcc -c bmp.c -Wall -Wextra -Wshadow -std=c11

clean:
	rm *.o main -f

.PHONY: clean all
