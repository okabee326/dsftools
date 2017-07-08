#!/bin/sh

gcc -Os -msse2 -fopenmp -ftree-vectorize -o dsf2raw.exe dsf2raw.c ../upconv/fileio.c -llibfftw3-3
cp dsf2raw.exe ./../upconvfe
