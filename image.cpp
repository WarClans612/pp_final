#include <opencv2/opencv.hpp>
#include <cstdlib>
#include "image.h"
using namespace cv;

int read_image(char *filename, int **r, int **g, int **b, int *width, int *height)
{
    Mat image;
    int image_size;

    image = imread(filename, IMREAD_COLOR);

    if(!image.data)
    {
        printf("No image data\n");
        return -1;
    }

    printf("\n******************************************\n");
    printf("Opening %s\n", filename);
    printf("Read image width:  %d\n", image.cols);
    printf("Read image height: %d\n", image.rows);

    *width = image.cols;
    *height = image.rows;
    image_size = image.cols * image.rows;


    *r = (int *) malloc(image_size * sizeof(int));
    *g = (int *) malloc(image_size * sizeof(int));
    *b = (int *) malloc(image_size * sizeof(int));

    for(int i = 0; i < *height; i++)
    {
        for(int j = 0; j < *width; j++)
        {
            // OpenCV is BGR
            (*b)[i*image.cols + j] = (int) image.at<Vec3b>(i,j)[0];
            (*g)[i*image.cols + j] = (int) image.at<Vec3b>(i,j)[1];
            (*r)[i*image.cols + j] = (int) image.at<Vec3b>(i,j)[2];
        }
    }

    printf("Read %s done.\n", filename);
    printf("******************************************\n");
    return 0;
}

void relu(int *r, int *g, int *b, int image_size)
{
    for(int i = 0; i < image_size; i++)
    {
        r[i] = std::max(r[i], 0);
        g[i] = std::max(g[i], 0);
        b[i] = std::max(b[i], 0);
    }

    return;
}

void free_image(int **r, int **g, int **b)
{
    free(*r);
    free(*g);
    free(*b);
    return;
}
