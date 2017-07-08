#!/bin/sh


g++ -O3 -fopenmp -mavx -march=native -static-libgcc -static-libstdc++ -o wav2dsf wav2dsf.cpp
