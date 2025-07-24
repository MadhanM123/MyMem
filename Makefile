# Name of your shared library
LIBNAME = mymem.so

# Source files
SRC = mymem.c

# Compiler and flags
CC = gcc
CFLAGS = -Wall -fPIC -shared

# Default target
all: $(LIBNAME)

$(LIBNAME): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Run test with LD_PRELOAD
run: $(LIBNAME)
	LD_PRELOAD=$(PWD)/$(LIBNAME) ls

# Clean build files
clean:
	rm -f $(LIBNAME)
