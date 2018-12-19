#ifndef __IMAGE_H__
#define __IMAGE_H__

int read_image(char *filename, int **r, int **g, int **b, int *width, int *height);
void relu(int *r, int *g, int *b, int image_size);
void free_image(int **r, int **g, int **b);

#endif
