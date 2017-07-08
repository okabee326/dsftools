#!/bin/sh

gcc -Os -msse2 -fopenmp -ftree-vectorize -o mconv.exe ../mconv.c ../fileio.c -llibfftw3-3
cp mconv.exe ../../multconvfe
