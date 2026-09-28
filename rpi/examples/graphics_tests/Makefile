CFLAGS=-Wall -O2
LIBS=

all: gfx_tests

gfx_tests: main.o
	$(CXX) main.o $(LIBS) -o gfx_tests 

main.o: ../../../macos/graphics_tests/graphics_tests/main.cpp ../../../src/bb_epaper.h
	$(CXX) $(CFLAGS) -c ../../../macos/graphics_tests/graphics_tests/main.cpp

clean:
	rm -rf *.o gfx_tests
