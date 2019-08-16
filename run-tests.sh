#!/usr/bin/env sh
g++ -std=c++11 -o test test-suite.cpp src/libtermbox.a src/display.cpp src/actor.cpp src/item.cpp src/input.cpp src/gameboard.cpp
./test
rm test
