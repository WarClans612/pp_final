#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            answer_sum += *(matA+(s_height+i)*width + s_width+j) * *(matB+i*size + j);
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
    int* padded_array = (int*)malloc( (new_width)*(new_height)*sizeof(int) );
    memset (padded_array, 0, (new_width)*(new_height)*sizeof(int));
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
    if (padding == 0){
        inputA = matA;
    }
    else {
        inputA = pad_array(matA, a_width, a_height, padding);
    }
    int new_width = (a_width+2*padding);
    int ans_width = (new_width-b_size)/step_size +1;
    int new_height = (a_height+2*padding);
    int ans_height = (new_height-b_size)/step_size + 1;
    int* answer = (int*)malloc( ans_width*ans_height*sizeof(int) );
    for(int i = 0; i < ans_height; ++i){
        for(int j = 0; j < ans_width; ++j){
            int new_val = dot_product(inputA, matB, b_size, j*step_size, i*step_size, new_width);
            *(answer+(i)*ans_width+(j)) = new_val;
        }
    }

    if (padding != 0) {
        free(inputA);
    }
    return answer;
}

int main(int argc, char** argv) {
    int input_matrix[1000000] = {0};
    for(int i = 0; i < 1000; ++i) {
        for(int j = 0; j < 1000; ++j) {
            input_matrix[i*1000+j] = i+1;
        }
    }

    int filter_matrix[16] = {
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4
    };

    int answer = dot_product(input_matrix, filter_matrix, 4, 0, 0, 1000);
    printf("Answer for dot product is %d\n", answer);
    printf("Corrent answer is 100\n");

    //----------------------------------------------------------------------------------------
    int inmatrix[100] = {0};
    for(int i = 0; i < 10; ++i) {
        for(int j = 0; j < 10; ++j) {
            inmatrix[i*10+j] = i+1;
        }
    }
    int* pad = pad_array(inmatrix, 10, 10, 2);
    printf("Answer for array padding:\n");
    for(int i = 0; i < 14; ++i) {
        for(int j = 0; j < 14; ++j) {
            printf("%5d", *(pad+i*14+j));
        }
        printf("\n");
    }
    free(pad);
    printf("Correct answer is:\n");
    printf("    0    0    0    0    0    0    0    0    0    0    0    0    0    0\n\
    0    0    0    0    0    0    0    0    0    0    0    0    0    0\n\
    0    0    1    1    1    1    1    1    1    1    1    1    0    0\n\
    0    0    2    2    2    2    2    2    2    2    2    2    0    0\n\
    0    0    3    3    3    3    3    3    3    3    3    3    0    0\n\
    0    0    4    4    4    4    4    4    4    4    4    4    0    0\n\
    0    0    5    5    5    5    5    5    5    5    5    5    0    0\n\
    0    0    6    6    6    6    6    6    6    6    6    6    0    0\n\
    0    0    7    7    7    7    7    7    7    7    7    7    0    0\n\
    0    0    8    8    8    8    8    8    8    8    8    8    0    0\n\
    0    0    9    9    9    9    9    9    9    9    9    9    0    0\n\
    0    0   10   10   10   10   10   10   10   10   10   10    0    0\n\
    0    0    0    0    0    0    0    0    0    0    0    0    0    0\n\
    0    0    0    0    0    0    0    0    0    0    0    0    0    0\n");

    //----------------------------------------------------------------------------------------
    int fil_matrix[16] = {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
    };

    int* conv = conv_layer(inmatrix, fil_matrix, 10, 10, 4, 2);
    printf("Answer for convolutional layer:\n");
    for(int i = 0; i < 11; ++i) {
        for(int j = 0; j < 11; ++j) {
            printf("%5d", *(conv+i*11+j));
        }
        printf("\n");
    }
    free(conv);
    printf("Correct answer is:\n");
    printf("    6    9   12   12   12   12   12   12   12    9    6\n\
   12   18   24   24   24   24   24   24   24   18   12\n\
   20   30   40   40   40   40   40   40   40   30   20\n\
   28   42   56   56   56   56   56   56   56   42   28\n\
   36   54   72   72   72   72   72   72   72   54   36\n\
   44   66   88   88   88   88   88   88   88   66   44\n\
   52   78  104  104  104  104  104  104  104   78   52\n\
   60   90  120  120  120  120  120  120  120   90   60\n\
   68  102  136  136  136  136  136  136  136  102   68\n\
   54   81  108  108  108  108  108  108  108   81   54\n\
   38   57   76   76   76   76   76   76   76   57   38\n");

    //----------------------------------------------------------------------------------------
    conv = conv_layer(inmatrix, fil_matrix, 10, 10, 4, 2, 3);
    printf("Answer for convolutional layer:\n");
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            printf("%5d", *(conv+i*4+j));
        }
        printf("\n");
    }
    free(conv);
    printf("Correct answer is:\n");
    printf("    6   12   12    9\n\
   28   56   56   42\n\
   52  104  104   78\n\
   54  108  108   81\n");
    
    return 0;
}