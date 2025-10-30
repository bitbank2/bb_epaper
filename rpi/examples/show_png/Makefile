CFLAGS=-ggdb -D__LINUX__ -Wall -O2 -I../../
LIBS=-g -lbb_epaper -lpngdec -lgpiod 

all: show_png

show_png: main.o cJSON.o cJSON_Utils.o
	$(CXX) main.o cJSON.o cJSON_Utils.o $(LIBS) -o show_png ;\
	sudo cp show_png /usr/local/bin

cJSON.o: cJSON.h cJSON.c
	$(CC) $(CFLAGS) -c cJSON.c

cJSON_Utils.o: cJSON_Utils.h cJSON_Utils.c
	$(CC) $(CFLAGS) -c cJSON_Utils.c

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o show_png
