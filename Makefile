CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++11 -O3 -flto -s
EXEC = sbg2gdl
OBJECTS = src/options.o src/move.o src/parser_events.o src/parser.o src/board.o src/piece.o src/goals.o src/game.o

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) src/main.cpp $(OBJECTS)

clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f $(EXEC)
