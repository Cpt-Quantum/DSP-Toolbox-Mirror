INC_DIR = ../inc
CC = gcc
CFLAGS = -Wall
LFLAGS = -lm

filter:
	$(CC) $(CFLAGS) main.c filter.c $(INC_DIR)/*.c -o main.o $(LFLAGS)
	./main.o

debug: 
	$(CC) $(CFLAGS) -g main.c filter.c $(INC_DIR)/*.c -o main.o $(LFLAGS)
