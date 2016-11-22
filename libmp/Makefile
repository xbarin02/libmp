CFLAGS=-std=c99 -pedantic -Wall -Wextra -Wconversion -march=native -O3 -D_POSIX_C_SOURCE=199309L
LDLIBS=-lrt
LIBNAME=mp
OBJ=libmp.o hsort.o
BIN=lib$(LIBNAME).a

-include ../Makefile.local

# CFLAGS += -g
# LDFLAGS += -g -rdynamic

.PHONY: all
all: $(BIN)

%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ $< -c

lib$(LIBNAME).a: $(OBJ)
	$(AR) -rs $@ $^
