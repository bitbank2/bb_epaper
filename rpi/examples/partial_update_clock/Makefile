CFLAGS=-D__LINUX__ -Wall -O2 -I../../
LIBS=-lbb_epaper -lgpiod 

all: pu_clock

pu_clock: main.o
	$(CXX) main.o $(LIBS) -o pu_clock 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o pu_clock
