#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef float float_t;

/**
 * @brief reads a pgm file and stores it as image (array)
 * 
 * @param image (space will be allocated inside)
 * @param height
 * @param width
 * @param negate (invert grayscale)
 */
int read_pgm(float_t** image, const char* filename, uint16_t *height, uint16_t *width, uint8_t negate) {
    FILE* image_file;
    image_file = fopen(filename, "r");

    if(!image_file) {
        return 1;
    }

    // read magic number
    char line[100];

    fgets(line, sizeof(line), image_file);

    if(strcmp(line, "P5\n") != 0) {
        return 1;
    }

    // skip comment
    fgets(line, sizeof(line), image_file);
    fgets(line, sizeof(line), image_file);

    sscanf(line, "%hu %hu\n", width, height);

    *image = (float_t*) malloc((*height)*(*width)*sizeof(float_t));

    // skip max gray value
    fgets(line, sizeof(line), image_file);

    uint8_t u8;

    for(int i = 0; i < (*height)*(*width); i++) {
        fread(&u8, sizeof(uint8_t), 1, image_file);
        (*image)[i] = (float_t) ((u8 / 255.0f) -0.5) * ((negate == 1) ? -2.0 : 2.0);
    }

    fclose(image_file);

    return 0;
}

/**
 * @brief makes a pgm image from an image (array)
 * 
 * @param height
 * @param width
 * @param image array which contains the image data
 * @param pgm_path full path to pgm image which should be written
 *
 * @return error (0 = success, 1 = error)
 */
int write_pgm(const float_t* image, const uint16_t height, const uint16_t width, const char* pgm_path) {

    // determine max gray value
    float_t max_gray_value = -1000;
    float_t min_gray_value = 1000;
    int row, column;

    for(row = 0; row < height; row++) {
        for(column = 0; column < width; column++) {
            if(image[row*width+column] > max_gray_value) {
                max_gray_value = image[row*width+column];
            }
            if(image[row*width+column] < min_gray_value) {
                min_gray_value = image[row*width+column];
            }
        }
    }

    float_t gray_value_range = fabsf(min_gray_value - max_gray_value);

    FILE* pgm_file; 
    pgm_file = fopen(pgm_path, "w+");

    if(!pgm_file) {
        return 1;
    }

    // magic_number
    fprintf(pgm_file, "P5\n");
    // comment
    fprintf(pgm_file, "# tiny-cnn\n");
    fprintf(pgm_file, "%u %u\n", width, height);
    
    fprintf(pgm_file, "%u\n", 255);

    for(row = 0; row < height; row++) {
        for(column = 0; column < width; column++) {
            // normalize
            uint8_t pixel = (uint8_t) ( ( (image[row*width+column]-min_gray_value) / gray_value_range ) * 255.0f );
            fputc(pixel, pgm_file);
        }
    }


    fclose(pgm_file);

    return 0;
}
