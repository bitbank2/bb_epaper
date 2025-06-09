CFLAGS=-D__LINUX__ -Wall -O2 -I../../
LIBS=-lbb_epaper -lgpiod 

all: hello_epaper

hello_epaper: main.o
	$(CXX) main.o $(LIBS) -o hello_epaper 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o hello_epaper
