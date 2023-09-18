CC=g++ 
CFLAGS=
SRCDIR= src
BINDIR= bin
SRCEXT= cpp
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


debug: build
	gdb $(BINDIR)/$(OUTPUT)