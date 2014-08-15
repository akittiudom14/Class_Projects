2D Image Convolution and Optimization (C)
=

In this project, I wrote code to optimize a formula for doing 2D convolution on images. 2D image convolution consists of taking a kernel matrix and sweeping it across an image (which essentially a matrix of pixels). Throughout this project, I became familiar with Intel SSE Instrinsics, OpenMP multithreading for parallelism, and general optimazation tricks. In part1.c, I utilized the SSE instructions along with register blocking, matrix padding, loop ordering, and loop unrolling to optimize from a baseline speed of 3 Gflops/s to about 10 Gflops/s. In part2.c I was able to optimize even further by additionally implementing cache blocking (based on the caches of the local machine), and most importantly, OpenMP multithreading to take advantage of the local 8 core machines. I was able to optimize the final code to over 70 Gflops/s!