#include "bmp.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "image_manager.h"
#include "shadow_manager.h"

void distribute_image(char* imagePath,int k, char* dir) {
    // Read the BMP image from the input file
    BMPImage* image = readBmpImage(imagePath);
    if (image == NULL) {
        fprintf(stderr, "Failed to read BMP image from file: %s\n", imagePath);
        return 1;
    }
    int with = image->header->width;
    int height = image->header->height;
    int imageSize = with * height;

    if ( k <= 4 && k >= 3) {
        //TODO: LSB4
    } else if (k <= 8 && k >= 5) {
        //TODO: LSB2
    } else {
        fprintf(stderr, "Invalid k value. Please choose a value between 3 and 8.\n");
        return;
    }
    generate_shadows(k, k, imageSize, image->image);


    // Free the allocated memory for the BMP image
    freeBmpImage(image);

}