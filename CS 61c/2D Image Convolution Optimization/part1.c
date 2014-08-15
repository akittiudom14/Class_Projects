#include <emmintrin.h>
#include <stdio.h>
#define KERNX 3 //this is the x-size of the kernel. It will always be odd.
#define KERNY 3 //this is the y-size of the kernel. It will always be odd.

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
    for(int y = 0; y < data_size_Y; y++){ // the y coordinate of theoutput location we're focusing on
		for(int x = 0; x < data_size_X; x++){ // the x coordinate of the output location we're focusing on
            p_in[toPaddedIndex(y, x, data_size_X, kern_cent_X, kern_cent_Y)] = in[toIndex(y, x, data_size_X)];
        }
    }
}

int conv2D(float* in, float* out, int data_size_X, int data_size_Y,
                    float* kernel)
{
    // the x coordinate of the kernel's center
    int kern_cent_X = (KERNX - 1)/2;
    // the y coordinate of the kernel's center
    int kern_cent_Y = (KERNY - 1)/2;

    __m128 kern_vec;
    __m128 work_vec1, work_vec2, work_vec3;
    __m128 out_vec1, out_vec2, out_vec3;
    
    // Flip the kernel, to keep things simple
    float f_kernel[KERNX*KERNY];
    flipKernel(kernel, f_kernel, KERNX*KERNY);

    // Create p_in, which is a zero-padded version of in
    int zero_pad_size = (data_size_Y + 2*kern_cent_Y)*(data_size_X + 2*kern_cent_X);
    float* p_in = (float*) calloc (zero_pad_size, sizeof(float));
    padMatrix(in, p_in, data_size_X, data_size_Y, kern_cent_X, kern_cent_Y);

    // main convolution loop: processes 12 columns of elements each iteration
	for(int x = 0; x < data_size_X/12*12; x = x + 12){ // the x coordinate of the output location we're focusing on
            // During each iteration of this for loop, process 12 columns of elements. 12 new columns are processed every iteration.
		for(int y = 0; y < data_size_Y; y += 1){ // the y coordinate of the output location we're focusing on, move down the column
            // During each iteration of this for loop, process 12 elements of a single row. The row # y is incremented after each iteration.
            out_vec1 = _mm_setzero_ps();
            out_vec2 = _mm_setzero_ps();
            out_vec3 = _mm_setzero_ps();
			for(int i = -kern_cent_Y; i <= kern_cent_Y; i++){ // kernel unflipped y coordinate
				for(int j = -kern_cent_X; j <= kern_cent_X; j++){ // kernel unflipped x coordinate
					
                    // f_kernel element is fixed here
                    kern_vec = _mm_load1_ps (f_kernel + (kern_cent_X + j) + (kern_cent_Y + i)*KERNX);
                    work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + i, x + j, data_size_X, kern_cent_X, kern_cent_Y));
                    work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + i, x + j + 4, data_size_X, kern_cent_X, kern_cent_Y));
                    work_vec3 = _mm_loadu_ps (p_in + toPaddedIndex(y + i, x + j + 8, data_size_X, kern_cent_X, kern_cent_Y));

                    out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec, work_vec1));
                    out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec, work_vec2));
                    out_vec3 = _mm_add_ps(out_vec3, _mm_mul_ps(kern_vec, work_vec3));
				}
			}
            _mm_storeu_ps((out + toIndex(y, x, data_size_X)), out_vec1);
            _mm_storeu_ps((out + toIndex(y, x + 4, data_size_X)), out_vec2);
            _mm_storeu_ps((out + toIndex(y, x + 8, data_size_X)), out_vec3);
		}
	}

    int k;
    int x = data_size_X/12*12;
    if (data_size_X - x >= 8) {
		for(int y = 0; y < data_size_Y; y += 1){ // the y coordinate of the output location we're focusing on, move down the column
            out_vec1 = _mm_setzero_ps();
            out_vec2 = _mm_setzero_ps();
			for(int i = -kern_cent_Y; i <= kern_cent_Y; i++){ // kernel unflipped y coordinate
				for(int j = -kern_cent_X; j <= kern_cent_X; j++){ // kernel unflipped x coordinate
					
                    // f_kernel element is fixed here
                    kern_vec = _mm_load1_ps (f_kernel + (kern_cent_X + j) + (kern_cent_Y + i)*KERNX);
                    work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + i, x + j, data_size_X, kern_cent_X, kern_cent_Y));
                    work_vec2 = _mm_loadu_ps (p_in + toPaddedIndex(y + i, x + j + 4, data_size_X, kern_cent_X, kern_cent_Y));

                    out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec, work_vec1));
                    out_vec2 = _mm_add_ps(out_vec2, _mm_mul_ps(kern_vec, work_vec2));
				}
			}
            _mm_storeu_ps((out + toIndex(y, x, data_size_X)), out_vec1);
            _mm_storeu_ps((out + toIndex(y, x + 4, data_size_X)), out_vec2);
		}
        k = x + 8;
	} else if (data_size_X - x >= 4) {
		for(int y = 0; y < data_size_Y; y += 1){ // the y coordinate of the output location we're focusing on, move down the column
            out_vec1 = _mm_setzero_ps();
			for(int i = -kern_cent_Y; i <= kern_cent_Y; i++){ // kernel unflipped y coordinate
				for(int j = -kern_cent_X; j <= kern_cent_X; j++){ // kernel unflipped x coordinate
					
                    // f_kernel element is fixed here
                    kern_vec = _mm_load1_ps (f_kernel + (kern_cent_X + j) + (kern_cent_Y + i)*KERNX);
                    work_vec1 = _mm_loadu_ps (p_in + toPaddedIndex(y + i, x + j, data_size_X, kern_cent_X, kern_cent_Y));

                    out_vec1 = _mm_add_ps(out_vec1, _mm_mul_ps(kern_vec, work_vec1));
				}
			}
            _mm_storeu_ps((out + toIndex(y, x, data_size_X)), out_vec1);
		}
        k = x + 4;
    } else {
        k = data_size_X/12*12;
    }

    for(int x = k; x < data_size_X; x = x + 1) { // the x coordinate of the output location we're focusing on
		for(int y = 0; y < data_size_Y; y += 1){ // the y coordinate of the output location we're focusing on, move down the column
			for(int i = -kern_cent_Y; i <= kern_cent_Y; i++){ // kernel unflipped y coordinate
				for(int j = -kern_cent_X; j <= kern_cent_X; j++){ // kernel unflipped x coordinate	
                    out[toIndex(y, x, data_size_X)] += *(f_kernel + (kern_cent_X + j) + (kern_cent_Y + i)*KERNX) * *(p_in + toPaddedIndex(y + i, x + j, data_size_X, kern_cent_X, kern_cent_Y));
				}
			}
		}
    }
    free(p_in);
	return 1;
}