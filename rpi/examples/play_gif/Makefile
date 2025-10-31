CFLAGS=-D__LINUX__ -Wall -O2 -I../../
LIBS=-lbb_epaper -lgpiod -lAnimatedGIF

all: play_gif

play_gif: main.o
	$(CXX) main.o $(LIBS) -o play_gif 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o play_gif
