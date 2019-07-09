#!/usr/bin/env sh
g++ -std=c++11 -Wall -Wextra -pedantic-errors -o rpg2 main.cpp libtermbox.a display.cpp actor.cpp
