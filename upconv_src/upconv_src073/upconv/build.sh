gcc -o upconv -O2 -march=native -fopenmp -ftree-vectorize upconv.c fileio.c makepath.c -lfftw3 -lm
