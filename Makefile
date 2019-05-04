CC=gcc
CXX=g++
CFLAGS=-O3 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -DVERBOSE
BIN=.

all: retro-video

retro-video: main.c
	$(CC) $(CFLAGS) main.c filterfunctions.c -lm -o $(BIN)/retro-video

clean:
	rm -r $(BIN)/retro-video