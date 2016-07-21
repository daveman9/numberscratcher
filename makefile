#The program Name (Name of the resulting executable)

PROG      = numberscratcher
VPATH     = src include
ODIR      = ./bin

#Name of subpaths inside Odir (Must be the same in ./include and ./src as well)
SUBPATHS  = .

#The Objects that are compiled
OBJS      = $(BASEOBS)

BASEOBS   = main.o Screen.o Field.o


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
	g++ -g -O2 -lpthread -o $@ $^ -lncurses

#compiling
$(ODIR)/%.o : %.cpp
	g++ -std=c++11 -Wall -O2 -c -o $@ $< $(addprefix -I./include/,$(SUBPATHS))

#Building the Directories if they don't exist
buildbin: | $(ODIR) $(addprefix $(ODIR)/,$(SUBPATHS))

$(ODIR):
	mkdir $(ODIR)

$(addprefix $(ODIR)/,$(SUBPATHS)):
	mkdir $@
