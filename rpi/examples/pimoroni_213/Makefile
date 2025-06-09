CFLAGS=-D__LINUX__ -Wall -O2 -I../../
LIBS=-lbb_epaper -lgpiod

all: pimoroni_213

pimoroni_213: main.o
	$(CXX) main.o $(LIBS) -o pimoroni_213 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o pimoroni_213
