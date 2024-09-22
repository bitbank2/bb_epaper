CFLAGS=-D__LINUX__ -Wall -O2 
LIBS = 

all: g5_demo

g5_demo: main.o
	$(CC) main.o -o g5_demo 

main.o: main.c ../../../src/Group5.h ../../../src/g5enc.inl
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -rf *.o g5_demo
