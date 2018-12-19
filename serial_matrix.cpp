#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include "image.h"
#include "load_filter.h"
using namespace std;

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
            // assert(((s_height+i)*width + s_width+j) >= 0);
            // assert(((s_height+i)*width + s_width+j) < a);
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
    int* padded_array = (int*) malloc(new_width * new_height * sizeof(int));
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
    int* answer = (int*) malloc(ans_width * ans_height * sizeof(int));

    for(int i = 0; i < ans_height; ++i) {
        for(int j = 0; j < ans_width; ++j) {
            int new_val = dot_product(inputA, matB, b_size, j*step_size, i*step_size, new_width);
            answer[i*ans_width + j] = new_val;
        }
    }

    if (padding != 0) {
        free(inputA);
    }
    return answer;
}

int main(int argc, char** argv) {

    if(argc < 2) {
        printf("Usage: ./serial_m <image_filename>\n");
        return 0;
    }

    int *image_r, *image_g, *image_b;
    int image_width, image_height;

    if(read_image(argv[1], &image_r, &image_g, &image_b, &image_width, &image_height) < 0) {
        printf("Error: can not open %s\n", argv[1]);
        return -1;
    }

    //----------------------------------------------------------------------------------------
    int fil_matrix[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };

    printf("\nDo convolution\n");

    int *conv_r, *conv_g, *conv_b;
    conv_r = conv_layer(image_r, fil_matrix, image_width, image_height, 3, 1);
    conv_g = conv_layer(image_g, fil_matrix, image_width, image_height, 3, 1);
    conv_b = conv_layer(image_b, fil_matrix, image_width, image_height, 3, 1);

    printf("Convolution done.\n");

    printf("\n******************************************\n");
    printf("Display the result in gray scale\n");
    printf("Use Relu function to make the result clear.\n");

    relu(conv_r, conv_g, conv_b, image_width * image_height);
    cv::Mat img(image_height, image_width, CV_8UC3, cv::Scalar(0, 0, 0));

    for(int i = 0; i < image_height; i++) {
        for(int j = 0; j < image_width; j++) {
            img.at<cv::Vec3b>(i, j)[0] = conv_b[i*image_width + j];
            img.at<cv::Vec3b>(i, j)[1] = conv_g[i*image_width + j];
            img.at<cv::Vec3b>(i, j)[2] = conv_r[i*image_width + j];
        }
    }

    // Display the result image
    cv::namedWindow("view",CV_WINDOW_NORMAL);
    cv::resizeWindow("view", 1280, 720);
    cv::imshow("view", img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    free_image(&image_r, &image_g, &image_b);
    free_image(&conv_r, &conv_g, &conv_b);
    printf("\ndone.\n");
    return 0;
}
