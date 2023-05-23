all: game

game:game.cpp
	g++ -Wall -Werror -g game.cpp -o game -lncurses

clean:
	rm -f game