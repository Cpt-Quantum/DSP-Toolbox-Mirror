TARGET = test
CC = gcc
CFLAGS = -Wall
CFLAGS += -c
CFLAGS += -fPIC

PATH_INCLUDE = ../inc

C_SRC = $(PATH_INCLUDE)/waveform_gen_float.c
C_SRC += $(PATH_INCLUDE)/constants_float.c
C_SRC += $(PATH_INCLUDE)/complex_float.c
C_SRC += py_bindings.c

O_FILES = waveform_gen_float.o
O_FILES += constants_float.o
O_FILES += complex_float.o
O_FILES += py_bindings.o

SO_FILE = clib.so

LINKER_FLAGS = -lm

build:
	$(CC) $(CFLAGS) $(C_SRC) $(LINKER_FLAGS)
	$(CC) -shared -o $(SO_FILE) $(O_FILES)

clean:
	rm $(O_FILES) $(SO_FILE)
