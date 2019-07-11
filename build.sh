#!/usr/bin/env sh
g++ -std=c++11 -Wall -Wextra -pedantic-errors -o rpg2 src/main.cpp src/libtermbox.a src/display.cpp src/actor.cpp src/item.cpp
