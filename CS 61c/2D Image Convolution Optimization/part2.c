#include <emmintrin.h>
#include <stdio.h>
#include <omp.h>
#define KERNX 3 //this is the x-size of the kernel. It will always be odd.
#define KERNY 3 //this is the y-size of the kernel. It will always be odd.
#define NUM_THREADS 8

int toIndex(int y, int x, int data_size_X) {
    return (x + y*data_size_X);
}

int toPaddedIndex(int y, int x, int data_size_X, int kern_cent_X, int kern_cent_Y) {
    return (kern_cent_Y*(data_size_X + 2*kern_cent_X) + y*(data_size_X+2*kern_cent_X) + (kern_cent_X + x));
}

void flipKernel(float* kernel, float* f_kernel, int n) {
    int k = 0;
    for (int i = n - 1; i >= 0; i--) {
        f_kernel[k] = kernel[i];
        k++;
    }
}

void padMatrix(float* in, float* p_in, int data_size_X, int data_size_Y, int kern_cent_X, int kern_cent_Y) {

    #pragma omp parallel for num_threads(NUM_THREADS) firstprivate(data_size_X, data_size_Y)
    for (int x = 0; x < data_size_X + 2; x++) {
        p_in[x] = 0;
    }

    #pragma omp parallel for num_threads(NUM_THREADS) firstprivate(data_size_X, data_size_Y, kern_cent_X, kern_cent_Y)
    for(int y = 0; y < data_size_Y; y++){ // the y coordinate of theoutput location we're focusing on
        p_in[(y+1)*(data_size_X+2)] = 0;
		for(int x = 0; x < data_size_X; x++) { // the x coordinate of the output location we're focusing on
            p_in[toPaddedIndex(y, x, data_size_X, kern_cent_X, kern_cent_Y)] = in[toIndex(y, x, data_size_X)];
        }
        p_in[(y+1)*(data_size_X+2) + (data_size_X+1)] = 0;
    }

    #pragma omp parallel for num_threads(NUM_THREADS) firstprivate(data_size_X, data_size_Y)
    for (int x = 0; x < data_size_X + 2; x++) {
        p_in[(data_size_X+2)*(data_size_Y + 1) + x] = 0;
    }
}

int conv2D(float* in, float* out, int data_size_X, int data_size_Y,
                    float* kernel)
{
    // the x coordinate of the kernel's center
    int kern_cent_X = (KERNX - 1)/2;
    // the y coordinate of the kernel's center
    int kern_cent_Y = (KERNY - 1)/2;
    
    // Flip the kernel, to keep things simple
    float f_kernel[KERNX*KERNY];
    flipKernel(kernel, f_kernel, KERNX*KERNY);

    // Create p_in, which is a zero-padded version of in
    int zero_pad_size = (data_size_Y + 2*kern_cent_Y)*(data_size_X + 2*kern_cent_X);
    float p_in[zero_pad_size];
    //float* p_in = (float*) calloc (zero_pad_size, sizeof(float));
    padMatrix(in, p_in, data_size_X, data_size_Y, kern_cent_X, kern_cent_Y);

    int num_rows = data_size_Y/NUM_THREADS;

    #pragma omp parallel num_threads(NUM_THREADS) firstprivate(data_size_X, data_size_Y, kern_cent_X, kern_cent_Y, num_rows)
    {
        int Pn = omp_get_thread_num();
        __m128 work_vec1, work_vec2, work_vec3;
        __m128 out_vec1, out_vec2, out_vec3;

        __m128 kern_vec1, kern_vec2, kern_vec3, kern_vec4, kern_vec5, kern_vec6, kern_vec7, kern_vec8, kern_vec9;
        kern_vec1 = _mm_load1_ps (f_kernel + 0);
        kern_vec2 = _mm_load1_ps (f_kernel + 1);
        kern_vec3 = _mm_load1_ps (f_kernel + 2);
        kern_vec4 = _mm_load1_ps (f_kernel + 3);
        kern_vec5 = _mm_load1_ps (f_kernel + 4);
        kern_vec6 = _mm_load1_ps (f_kernel + 5);
        kern_vec7 = _mm_load1_ps (f_kernel + 6);
        kern_vec8 = _mm_load1_ps (f_kernel + 7);
        kern_vec9 = _mm_load1_ps (f_kernel + 8);

        // main convolution loop: processes 12 columns of elements each iteration
        for(int x = 0; x < data_size_X/12*12; x = x + 12){ // the x coordinate of the output location we're focusing on
                // During each iteration of this for loop, process 12 columns of elements. 12 new columns are processed every iteration.
            for(int y = Pn*num_rows; y < num_rows*(Pn+1); y += 1){ // the y coordinate of the output location we're focusing on, move down the column
                // During each iteration of this for loop, process 12 elements of a single row. The row # y is incremented after each iteration.
                out_vec1 = _mm_setzero_ps();
                out_vec2 = _mm_setzero_ps();
                out_vec3 = _mm_setzero_ps();
                        
                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x - 1, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x - 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x - 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec1, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec1, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec1, work_vec3));

                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 0, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 0 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 0 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec2, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec2, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec2, work_vec3));

                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 1, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec3, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec3, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec3, work_vec3));

                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 1, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec4, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec4, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec4, work_vec3));

                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 0, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 0 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 0 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec5, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec5, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec5, work_vec3));

                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x + 1, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x + 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x + 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec6, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec6, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec6, work_vec3));

                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x - 1, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x - 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x - 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec7, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec7, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec7, work_vec3));

                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 0, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 0 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 0 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec8, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec8, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec8, work_vec3));

                work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 1, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
                work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

                out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec9, work_vec1));
                out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec9, work_vec2));
                out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec9, work_vec3));

                _mm_storeu_ps((out + toIndex(y, x, data_size_X)), out_vec1);
                _mm_storeu_ps((out + toIndex(y, x + 4, data_size_X)), out_vec2);
                _mm_storeu_ps((out + toIndex(y, x + 8, data_size_X)), out_vec3);
            }
        }
    }

    // May instead want to ASSIGN CHUNKS OF ROWS TO A CERTAIN THREAD instead of relying on #pragma omp for
    #pragma omp parallel for num_threads(NUM_THREADS) firstprivate(data_size_X, data_size_Y, kern_cent_X, kern_cent_Y, f_kernel)
    for(int x = data_size_X/12*12; x < data_size_X; x = x + 1) { // the x coordinate of the output location we're focusing on
        for(int y = 0; y < data_size_Y; y += 1){ // the y coordinate of the output location we're focusing on, move down the column
            out[toIndex(y, x, data_size_X)] = p_in[toPaddedIndex(y - 1, x - 1, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[0]
                        +   p_in[toPaddedIndex(y - 1, x + 0, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[1]
                        +   p_in[toPaddedIndex(y - 1, x + 1, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[2]
                        +   p_in[toPaddedIndex(y + 0, x - 1, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[3]
                        +   p_in[toPaddedIndex(y + 0, x - 0, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[4]
                        +   p_in[toPaddedIndex(y + 0, x + 1, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[5]
                        +   p_in[toPaddedIndex(y + 1, x - 1, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[6]
                        +   p_in[toPaddedIndex(y + 1, x + 0, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[7]
                        +   p_in[toPaddedIndex(y + 1, x + 1, data_size_X, kern_cent_X, kern_cent_Y)] * f_kernel[8];
        }
    }

    // May want to add threading to this scenario to cover last <8 rows
    // Also may want to switch up the order of the tailing chunks
    __m128 work_vec1, work_vec2, work_vec3;
    __m128 out_vec1, out_vec2, out_vec3;

    __m128 kern_vec1, kern_vec2, kern_vec3, kern_vec4, kern_vec5, kern_vec6, kern_vec7, kern_vec8, kern_vec9;
    kern_vec1 = _mm_load1_ps (f_kernel + 0);
    kern_vec2 = _mm_load1_ps (f_kernel + 1);
    kern_vec3 = _mm_load1_ps (f_kernel + 2);
    kern_vec4 = _mm_load1_ps (f_kernel + 3);
    kern_vec5 = _mm_load1_ps (f_kernel + 4);
    kern_vec6 = _mm_load1_ps (f_kernel + 5);
    kern_vec7 = _mm_load1_ps (f_kernel + 6);
    kern_vec8 = _mm_load1_ps (f_kernel + 7);
    kern_vec9 = _mm_load1_ps (f_kernel + 8);

    // main convolution loop: processes 12 columns of elements each iteration
    for(int x = 0; x < data_size_X/12*12; x = x + 12){ // the x coordinate of the output location we're focusing on
            // During each iteration of this for loop, process 12 columns of elements. 12 new columns are processed every iteration.
        for(int y = NUM_THREADS*num_rows; y < data_size_Y; y += 1){ // the y coordinate of the output location we're focusing on, move down the column
            // During each iteration of this for loop, process 12 elements of a single row. The row # y is incremented after each iteration.
            out_vec1 = _mm_setzero_ps();
            out_vec2 = _mm_setzero_ps();
            out_vec3 = _mm_setzero_ps();
                    
            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x - 1, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x - 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x - 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec1, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec1, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec1, work_vec3));

            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 0, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 0 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 0 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec2, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec2, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec2, work_vec3));

            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 1, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y - 1, x + 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec3, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec3, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec3, work_vec3));

            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 1, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec4, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec4, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec4, work_vec3));

            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 0, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 0 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x - 0 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec5, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec5, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec5, work_vec3));

            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x + 1, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x + 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 0, x + 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec6, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec6, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec6, work_vec3));

            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x - 1, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x - 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x - 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec7, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec7, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec7, work_vec3));

            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 0, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 0 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 0 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec8, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec8, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec8, work_vec3));

            work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 1, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 1 + 4, data_size_X, kern_cent_X, kern_cent_Y));
            work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + 1, x + 1 + 8, data_size_X, kern_cent_X, kern_cent_Y));

            out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec9, work_vec1));
            out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec9, work_vec2));
            out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec9, work_vec3));

            _mm_storeu_ps((out + toIndex(y, x, data_size_X)), out_vec1);
            _mm_storeu_ps((out + toIndex(y, x + 4, data_size_X)), out_vec2);
            _mm_storeu_ps((out + toIndex(y, x + 8, data_size_X)), out_vec3);
        }
    }
	return 1;
}
