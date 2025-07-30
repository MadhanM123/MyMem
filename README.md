# MyMem is a very basic memory allocator.

I've implemented calloc(), realloc(), malloc(), and free().

#### Compilation ####

Run "make all" to compile

#### Running ####

Run "make run" to see mymem being used for a simple ls command

Set "export LD_PRELOAD=$PWD/mymem.so". This will ensure that all future commands in the current shell use mymem for memory tasks.

#### Resetting ####

Run "unset LD_PRELOAD" to go back to the system memory allocation.