#!/bin/sh
gcc -o engine.o -c engine.c
gcc -o camera.o -c camera.c
gcc -o input.o -c input.c
gcc -o rollercoaster.o -c rollercoaster.c

gcc -Wall -o rollercoaster engine.o camera.o input.o rollercoaster.o main.c -lglut32cu -lglu32 -lopengl32

rm engine.o camera.o input.o rollercoaster.o