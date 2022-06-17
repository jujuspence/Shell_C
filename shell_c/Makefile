# provide a makefile with a target called shell_jr that 
# creates the executable shell_jr
CC = gcc
CFLAGS = -ansi -Wall -g -O0 -Wwrite-strings -Wshadow \
         -pedantic-errors -fstack-protector-all
PROGS = shell_jr

.PHONY: all clean

all: $(PROGS)

clean: 
        rm -f *.o $(PROGS)

shell_jr: shell_jr.o
        $(CC) -o shell_jr shell_jr.o

shell_jr.o: shell_jr.c
        $(CC) $(CFLAGS) -c shell_jr.c 
