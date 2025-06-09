CFLAGS=-D__LINUX__ -Wall -O2 -I../../
LIBS=-lbb_epaper -lgpiod

all: compressed_font

compressed_font: main.o
	$(CXX) main.o $(LIBS) -o compressed_font 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o compressed_font
