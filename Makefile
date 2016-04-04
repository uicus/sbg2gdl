CXX = g++
CXXFLAGS = -Wall -W -std=c++11 -O2
EXEC = sbg2gdl
OBJECTS = options.o move.o parser.o board.o piece.o goals.o

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) main.cpp $(OBJECTS)

clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f $(EXEC)
