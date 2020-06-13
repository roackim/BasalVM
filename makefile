
appname := main

#LIBS= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
LIBS=
CXXFLAGS= -fmax-errors=2 -Wall -Wextra -Wpedantic

srcfiles := $(shell find . -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	g++ $(CXXFLAGS) $(LIBS) -o $(appname) $(objects)

clean:
	rm -f $(objects)

dist-clean: clean
	rm -f *~ .depend

include .depend
