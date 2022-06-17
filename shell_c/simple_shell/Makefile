	CC = gcc 
	CFLAGS = -ansi -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors -fstack-protector-all 
	PROGS = d8sh  
	 
	.PHONY: all clean 
	 
	all: $(PROGS) 
	 
	clean:  
	        rm -f *.o $($PROGS) a.out 
	 
	d8sh: d8sh.o parser.tab.o lexer.o executor.o 
	        $(CC) $(CFLAGS) -lreadline -o d8sh d8sh.o lexer.o executor.o parser.tab.o 
	 
	lexer.o: lexer.c lexer.h 
	        $(CC) $(CFLAGS) -c lexer.c 
	parser.tab.o: parser.tab.c parser.tab.h 
	        $(CC) $(CFLAGS) -c parser.tab.c 
	executor.o: executor.c executor.h 
	        $(CC) $(CFLAGS) -c executor.c 
	d8sh.o: d8sh.c 
	        $(CC) $(CFLAGS) -c d8sh.c 
