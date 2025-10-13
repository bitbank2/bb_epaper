CFLAGS=-D__LINUX__ -Wall -O2 -I../../
LIBS=-lbb_epaper -lpngdec -lgpiod 

all: show_png

show_png: main.o
	$(CXX) main.o $(LIBS) -o show_png 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o show_png
