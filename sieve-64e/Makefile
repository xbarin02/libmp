CFLAGS=-std=c99 -pedantic -Wall -Wextra -march=native -O3 -D_POSIX_C_SOURCE=199309L -I../libmp
LDLIBS=-lrt -L../libmp -lmp
BIN=sieve-64e

-include ../Makefile.local

# CFLAGS += -g
# LDFLAGS += -g -rdynamic

.PHONY: all
all: $(BIN)
