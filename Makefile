CXX = g++
CXXFLAGS = -Wall -W -std=c++11 -O2
EXEC = sbg2gdl
OBJECTS = src/options.o src/move.o src/parser.o src/board.o src/piece.o src/goals.o

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) src/main.cpp $(OBJECTS)

clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f $(EXEC)
