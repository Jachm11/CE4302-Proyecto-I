CC=gcc 
CFLAGS= -w
SRCDIR= src
BINDIR= bin
SRCEXT= c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(SOURCES:.$(SRCEXT)=.o)
OUTPUT= backend 

all: exec 

build: $(OBJECTS) 
		@mkdir -p $(BINDIR)
		$(CC) $(CFLAGS) $^  -o $(BINDIR)/$(OUTPUT) -lpthread
		@$(RM) -r $(OBJECTS)

%.o: %.$(SRCEXT)
	$(CC) $(CFLAGS) -c $< -o $@

exec: build
	./$(BINDIR)/$(OUTPUT)

clean:
	@$(RM) -r src/*.o

debug: build
	gdb $(BINDIR)/$(OUTPUT)