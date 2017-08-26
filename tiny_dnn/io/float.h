#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef float float_t;

// TODO
int read_float(float_t** image, const char* filename, uint16_t *height, uint16_t *width) {
 
    FILE* float_file; 
    float_file = fopen(filename, "r");

    if(!float_file) {
        return 1;
    }

    char buffer[100];

    // read magic number 
    fgets(buffer, 100, float_file);

    if(strncmp(buffer, "FF", 2) != 0) {
        fclose(float_file);
        return 1;
    }

    // read height and width
    fgets(buffer, 100, float_file);
    *height = atoi(buffer);

    fgets(buffer, 100, float_file);
    *width = atoi(buffer);

    // store data from file to image
    *image = (float_t*) malloc((*height)*(*width)*sizeof(float_t));

    uint16_t i;

    for(i = 0; i < (*height)*(*width); i++) {

        fscanf(float_file, "%a", &(*image)[i]);
    }

    fclose(float_file);

    return 0;
}

// write image (matrix) to file with float values
int write_float(const float_t* image, const uint16_t height, const uint16_t width, const char* float_path) {

    int row, column;

    FILE* float_file; 
    float_file = fopen(float_path, "w+");

    if(!float_file) {
        return 1;
    }

    // magic_number
    fprintf(float_file, "FF\n");

    // dimensions
    fprintf(float_file, "%u\n", height);
    fprintf(float_file, "%u\n", width);
    
    for(row = 0; row < height; row++) {
        for(column = 0; column < width; column++) {
            fprintf(float_file, "%a\n", image[row*width+column]);
        }
    }
    
    fclose(float_file);

    return 0;
}
