flags = -std=c++17 -Wall -Wextra -pedantic-errors

rpg2: main.o display.o actor.o item.o input.o gameboard.o template.o
	clang++ $(flags) -o rpg2 src/libtermbox.a main.o display.o actor.o item.o input.o gameboard.o template.o

main.o: src/main.cpp
	clang++ $(flags) -c src/main.cpp -o main.o

display.o: src/display.cpp
	clang++ $(flags) -c src/display.cpp -o display.o

actor.o: src/actor.cpp
	clang++ $(flags) -c src/actor.cpp -o actor.o

item.o: src/item.cpp
	clang++ $(flags) -c src/item.cpp -o item.o

input.o: src/input.cpp
	clang++ $(flags) -c src/input.cpp -o input.o

gameboard.o: src/gameboard.cpp
	clang++ $(flags) -c src/gameboard.cpp -o gameboard.o

template.o: src/template.cpp
	clang++ $(flags) -c src/template.cpp -o template.o

clean:
	rm rpg2 *.o *~
