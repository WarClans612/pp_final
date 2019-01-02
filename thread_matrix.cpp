#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include "image.h"
#include "filter.h"
using namespace std;

static int num_threads;

/*----------------------------------------------------------------------------------------
    Matrix dot product calculation
    Input matrix is expected to be 1-dimensional
    matA and matB is input picture matrix and filter matrix
    s_width and s_height is expected to be coordinate is the picture,
    where the upper left is the starting coordinate of the calculation
    Width represent the width of input picture matrix
    Boundary check will not be done, so caller is expected to check beforehand
----------------------------------------------------------------------------------------*/
int dot_product(int* matA, int* matB, int size, int s_width, int s_height, int width) {
    int answer_sum = 0;
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            answer_sum += matA[(s_height+i)*width + s_width+j] * matB[i*size + j];
        }
    }
    return answer_sum;
}

/*----------------------------------------------------------------------------------------
    Function to pad array
    Returned a malloc array of appropriate size
    Need to be freed by caller
----------------------------------------------------------------------------------------*/
int* pad_array(int* input, int width, int height, int padding) {
    int new_width = width+2*padding;
    int new_height = height+2*padding;
    int* padded_array = new int [new_width * new_height * sizeof(int)];
    memset (padded_array, 0, new_width * new_height * sizeof(int));

    for(int i = padding; i < new_height-padding; ++i) {
        for(int j = padding; j < new_width-padding; ++j) {
            *(padded_array+i*new_width+j) = *(input+(i-padding)*width+(j-padding));
        }
    }
    return padded_array;
}

/*----------------------------------------------------------------------------------------
    Single layer of convolutional layer
----------------------------------------------------------------------------------------*/
int* conv_layer(int* matA, int* matB, int a_width, int a_height, int b_size, int padding = 0, int step_size = 1) {
    int* inputA;

    if (padding == 0) {
        inputA = matA;
    }
    else {
        inputA = pad_array(matA, a_width, a_height, padding);
    }

    int new_width = a_width + 2*padding;
    int ans_width = (new_width-b_size)/step_size + 1;
    int new_height = a_height + 2*padding;
    int ans_height = (new_height-b_size)/step_size + 1;
    int* answer = new int [ans_width * ans_height * sizeof(int)];

    int new_val;
    for(int i = 0; i < ans_height; ++i) {
        for(int j = 0; j < ans_width; ++j) {
            new_val = dot_product(inputA, matB, b_size, j*step_size, i*step_size, new_width);
            answer[i*ans_width + j] = new_val;
        }
    }

    if (padding != 0) {
        delete [] inputA;
    }
    return answer;
}

int main(int argc, char** argv) {

    int show_result = 0;

    if(argc < 4) {
        printf("Usage: ./serial_m <image_filename> <filter_filename> <number_of_threads> [show_result: 1 | 0]\n");
        return 0;
    }

    if(argc >= 5)
        show_result = atoi(argv[4]);

    num_threads = atoi(argv[3]);
    omp_set_num_threads(num_threads);
    printf("\n******************************************\n");
    printf("Using %d cores with OpenMP\n", num_threads);
    printf("******************************************\n");


    int *image_r, *image_g, *image_b;
    int image_width, image_height;

    if(read_image(argv[1], &image_r, &image_g, &image_b, &image_width, &image_height) < 0) {
        printf("Error: can not open %s\n", argv[1]);
        return -1;
    }

    //----------------------------------------------------------------------------------------
    int num_filters;
    int *fil_size;
    int **fil_matrix;
    load_filter(argv[2], &num_filters, &fil_matrix, &fil_size);

    struct timeval t_begin, t_end;

    printf("\n******************************************\n");
    printf("Do convolution\n");

    gettimeofday(&t_begin, 0);

    int **conv_r, **conv_g, **conv_b;
    conv_r = new int* [num_filters];
    conv_g = new int* [num_filters];
    conv_b = new int* [num_filters];

    #pragma omp parallel for
    for(int i = 0; i < num_filters; i++)
    {
        conv_r[i] = conv_layer(image_r, fil_matrix[i], image_width, image_height, fil_size[i], (fil_size[i]-1) / 2);
        conv_g[i] = conv_layer(image_g, fil_matrix[i], image_width, image_height, fil_size[i], (fil_size[i]-1) / 2);
        conv_b[i] = conv_layer(image_b, fil_matrix[i], image_width, image_height, fil_size[i], (fil_size[i]-1) / 2);
    }

    gettimeofday(&t_end, 0);

    printf("Convolution done.\n");
    printf("******************************************\n");

    //-----------------------------------------------------
    for(int i = 0; i < num_filters; i++)
    {
        printf("filter %d:\n", i);
        print_filter(fil_matrix[i], fil_size[i]);
        if(show_result)
            show_image(conv_r[i], conv_g[i], conv_b[i], image_width, image_height);
        free_image(conv_r[i], conv_g[i], conv_b[i]);
    }

    if(show_result)
        cv::destroyAllWindows();

    delete [] conv_r;
    delete [] conv_g;
    delete [] conv_b;
    free_filter(num_filters, fil_matrix, fil_size);
    //-----------------------------------------------------

    int sec = t_end.tv_sec - t_begin.tv_sec;
    int usec = t_end.tv_usec - t_begin.tv_usec;
    printf("\nSpend %f sec\n", (sec*1000+(usec/1000.0))/1000);
    printf("done.\n");
    return 0;
}
