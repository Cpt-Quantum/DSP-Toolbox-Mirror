INC_DIR = ../inc
CC = gcc
CFLAGS = -Wall
LFLAGS = -lm

fft:
	$(CC) $(CFLAGS) main.c fft.c $(INC_DIR)/*.c -o main.o $(LFLAGS)
	./main.o

debug: 
	$(CC) $(CFLAGS) -g main.c fft.c $(INC_DIR)/*.c -o main.o $(LFLAGS)
