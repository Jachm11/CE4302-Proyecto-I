CC=gcc 
CFLAGS= -w
SRCDIR= src
BINDIR= bin
SRCEXT= c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(SOURCES:.$(SRCEXT)=.o)
OUTPUT= backend_debug 


debug:
	touch  results/sim_result.txt
	./$(BINDIR)/$(OUTPUT) debug > results/sim_result.txt