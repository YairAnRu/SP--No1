all: clean compile run

compile:
	g++ -std=c++11 -Wall -Wextra *.cpp -o build/exercise
run:
	build/exercise
clean:
	rm -f build/exercise
	rm -f build/dexercise
debug:
	g++ -std=c++11 *.cpp -g -o build/dexercise
	gdb build/dexercise