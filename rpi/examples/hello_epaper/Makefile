CFLAGS=-D_LINUX_ -Wall -O2 -I../../
LIBS=-lgpiod 

all: hello_epaper

hello_epaper: main.o bb_epaper.o
	$(CXX) main.o bb_epaper.o $(LIBS) -o hello_epaper 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

bb_epaper.o: ../../../src/bb_epaper.cpp ../../../src/bb_epaper.h
	$(CXX) $(CFLAGS) -c ../../../src/bb_epaper.cpp

clean:
	rm -rf *.o hello_epaper
