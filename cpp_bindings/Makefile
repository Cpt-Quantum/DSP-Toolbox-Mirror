CC = g++
CFLAGS = -Wall -g

PATH_INCLUDE = ../inc

C_SRC = cpp_wrapper.cpp
C_SRC += $(PATH_INCLUDE)/waveform_gen.c
C_SRC += $(PATH_INCLUDE)/constants.c
C_SRC += $(PATH_INCLUDE)/complex.c

LINKER_FLAGS = -lm

test:
	$(CC) $(CFLAGS) test.cpp $(C_SRC) -o test.o $(LINKER_FLAGS)
filter:
	$(CC) $(CFLAGS) filter_test.cpp $(C_SRC) -o filter.o $(LINKER_FLAGS)

clean:
	-rm $(TARGET).o
	-rm output_file.csv
