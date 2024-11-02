CFLAGS=-D_LINUX_ -Wall -O2 -I../../
LIBS=-lgpiod 

all: compressed_font

compressed_font: main.o bb_epaper.o
	$(CXX) main.o bb_epaper.o $(LIBS) -o compressed_font 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

bb_epaper.o: ../../../src/bb_epaper.cpp ../../../src/bb_epaper.h
	$(CXX) $(CFLAGS) -c ../../../src/bb_epaper.cpp

clean:
	rm -rf *.o compressed_font
