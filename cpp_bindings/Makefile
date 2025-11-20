TARGET = test
CC = g++
CFLAGS = -Wall

PATH_INCLUDE = ../inc

C_SRC = test.cpp
C_SRC += cpp_wrapper.cpp
C_SRC += $(PATH_INCLUDE)/waveform_gen.c
C_SRC += $(PATH_INCLUDE)/constants.c
C_SRC += $(PATH_INCLUDE)/complex.c

LINKER_FLAGS = -lm

test:
	$(CC) $(CFLAGS) $(C_SRC) -o $(TARGET).o $(LINKER_FLAGS)

clean:
	rm $(TARGET).o
