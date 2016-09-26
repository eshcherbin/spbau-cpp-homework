CPP=g++
FLAGS=-std=c++11 -Wall -Wextra -Wshadow -O2

all: bin/huffman

bin/huffman: bin/main.o bin/huffman.o
	mkdir -p bin
	$(CPP) $(FLAGS) bin/main.o bin/huffman.o -o bin/huffman

bin/main.o: src/main.cpp src/huffman.h
	mkdir -p bin
	$(CPP) $(FLAGS) -c src/main.cpp -o bin/main.o

bin/huffman.o: src/huffman.cpp src/huffman.h
	mkdir -p bin
	$(CPP) $(FLAGS) -c src/huffman.cpp -o bin/huffman.o

test: bin/test.o bin/huffman_test.o bin/huffman.o bin/autotest.o
	mkdir -p bin
	$(CPP) $(FLAGS) bin/test.o bin/huffman_test.o bin/huffman.o bin/autotest.o -o bin/test

bin/test.o: test/test.cpp test/huffman_test.h test/autotest.h
	mkdir -p bin
	$(CPP) $(FLAGS) -c test/test.cpp -o bin/test.o

bin/huffman_test.o: test/huffman_test.cpp test/huffman_test.h src/huffman.h
	mkdir -p bin
	$(CPP) $(FLAGS) -c test/huffman_test.cpp -o bin/huffman_test.o

bin/autotest.o: test/autotest.cpp test/autotest.h src/huffman.h
	mkdir -p bin
	$(CPP) $(FLAGS) -c test/autotest.cpp -o bin/autotest.o

clean:
	rm bin/* -f

.PHONY: clean all
