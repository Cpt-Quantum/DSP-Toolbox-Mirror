CC = g++
CFLAGS = -Wall -g

PATH_INCLUDE = ../inc

C_SRC = $(PATH_INCLUDE)/waveform_gen_float.c
C_SRC += $(PATH_INCLUDE)/constants_float.c
C_SRC += $(PATH_INCLUDE)/complex_float.c

LINKER_FLAGS = -lm

test:
	$(CC) $(CFLAGS) test.cpp $(C_SRC) -o test.o $(LINKER_FLAGS)

clean:
	-rm $(TARGET).o
	-rm output_file.csv
