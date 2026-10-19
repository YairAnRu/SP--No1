CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
OUT = build/exercise

all: compile run

compile:
	$(CXX) $(CXXFLAGS) main.cpp -o $(OUT)

run:
	./$(OUT)

debugvs:
	$(CXX) -std=c++11 -g main.cpp -o build/dexercise

clean:
	$(RM) build/exercise build/dexercise

.PHONY: all compile run debugvs clean