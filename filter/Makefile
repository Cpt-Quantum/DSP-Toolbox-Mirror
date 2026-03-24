INC_DIR = ../inc
CC = gcc
CFLAGS = -Wall
LFLAGS = -lm

C_FILES = main.c
C_FILES += iir.c
C_FILES += fir.c
C_FILES += main_inc.c
C_FILES += $(INC_DIR)/waveform_gen_float.c
C_FILES += $(INC_DIR)/spectrum_gen_float.c

filter:
	$(CC) $(CFLAGS) $(C_FILES) -o main.o $(LFLAGS)
	./main.o

debug: 
	$(CC) $(CFLAGS) -g $(C_FILES) -o main.o $(LFLAGS)
