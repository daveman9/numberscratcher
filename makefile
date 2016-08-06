#The program Name (Name of the resulting executable)

PROG      = tenpair
VPATH     = src include
ODIR      = ./bin
SHAREFLAGS= -g -pipe
CFLAGS    = $(SHAREFLAGS) -std=c++11 -Wall
LINKFLAGS = $(SHAREFLAGS) -lncurses
COMPILER  = g++

#Name of subpaths inside Odir (Must be the same in ./include and ./src as well)
SUBPATHS  = .

#The Objects that are compiled
OBJS      = $(BASEOBS)

BASEOBS   = main.o Field.o Game.o


OPROG = $(addprefix $(ODIR)/, $(PROG))

run : all
	$(OPROG)

all : buildbin $(OPROG)

clean:
	find bin -name '*.o' -delete
	rm -f $(OPROG)

.PHONY: run all clean

#linking
$(OPROG): $(addprefix $(ODIR)/, $(OBJS))
	$(COMPILER) -o $@ $^ $(LINKFLAGS)

#compiling
$(ODIR)/%.o : %.cpp
	$(COMPILER) $(CFLAGS) -c -o $@ $< $(addprefix -I./include/,$(SUBPATHS))

#Building the Directories if they don't exist
buildbin: | $(ODIR) $(addprefix $(ODIR)/,$(SUBPATHS))

$(ODIR):
	mkdir $(ODIR)

$(addprefix $(ODIR)/,$(SUBPATHS)):
	mkdir $@
