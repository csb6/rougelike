#!/usr/bin/env sh
#Add debug flag when using static analyzer
clang++ -std=c++11 -Wall -Wextra -pedantic-errors -o rpg2 src/main.cpp src/libtermbox.a src/display.cpp src/actor.cpp src/item.cpp src/input.cpp src/gameboard.cpp
