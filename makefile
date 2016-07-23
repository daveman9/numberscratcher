#The program Name (Name of the resulting executable)

PROG      = numberscratcher
VPATH     = src include
ODIR      = ./bin
SHAREFLAGS= -g -O2
CFLAGS    = $(SHAREFLAGS) -std=c++11 -Wall
LINKFLAGS = $(SHAREFLAGS) -lpthread -lncurses
COMPILER  = g++

#Name of subpaths inside Odir (Must be the same in ./include and ./src as well)
SUBPATHS  = .

#The Objects that are compiled
OBJS      = $(BASEOBS)

BASEOBS   = main.o Field.o Game.o


OPROG = $(addprefix $(ODIR)/, $(PROG))

run : all
	$(OPROG) 2> outfifo

all : buildbin $(OPROG)

clean:
	find bin -name '*.o' -delete
	rm -f $(OPROG)

.PHONY: run all clean

#linking
$(OPROG): $(addprefix $(ODIR)/, $(OBJS))
	$(COMPILER) $(LINKFLAGS) -o $@ $^

#compiling
$(ODIR)/%.o : %.cpp
	$(COMPILER) $(CFLAGS) -c -o $@ $< $(addprefix -I./include/,$(SUBPATHS))

#Building the Directories if they don't exist
buildbin: | $(ODIR) $(addprefix $(ODIR)/,$(SUBPATHS))

$(ODIR):
	mkdir $(ODIR)

$(addprefix $(ODIR)/,$(SUBPATHS)):
	mkdir $@
