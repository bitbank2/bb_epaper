CFLAGS=-c -Wall -O2 -ggdb -D__LINUX__ -I../src
LIBS = -lm -lpthread

all: libbb_epaper.a

libbb_epaper.a: bb_epaper.o
	ar -rc libbb_epaper.a bb_epaper.o ;\
	sudo cp libbb_epaper.a /usr/local/lib ;\
	sudo cp ../src/bb_epaper.h /usr/local/include

bb_epaper.o: ../src/bb_epaper.cpp ../src/bb_epaper.h ../src/bb_ep.inl ../src/bb_ep_gfx.inl ../src/rpi_io.inl
	$(CXX) $(CFLAGS) ../src/bb_epaper.cpp

clean:
	rm *.o libbb_epaper.a
