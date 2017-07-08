#!/bin/sh
g++ -O3 -fopenmp -mavx -march=native -static-libgcc -static-libstdc++ -o dsf2wav dsf2wav.cpp
