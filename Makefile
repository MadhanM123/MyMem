# Name of your shared library
LIBNAME = mymem.so

# Source files
SRC = mymem.c

# Compiler and flags
CC = gcc
CFLAGS = -Wall -fPIC -shared

LIBDIR := $(shell pwd)
LDLIB := $(LIBDIR)/$(LIBNAME)

# Default target
all: $(LIBNAME)

$(LIBNAME): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Run test with LD_PRELOAD
run: $(LIBNAME)
	LD_PRELOAD=$(LDLIB) ls

# Clean build files
clean:
	rm -f $(LIBNAME)
