#!/bin/sh

g++ -c ../src/handle_input.cpp -o handle_input.o
g++ -c ../src/check_value.cpp -o check_value.o
gcc -c ../src/main.c -lusb-1.0 -o main.o
g++ main.o handle_input.o check_value.o -lusb-1.0 -o set_lights -Wall
