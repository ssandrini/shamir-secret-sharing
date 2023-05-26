#include "bmp.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define OFFSET_ZERO 0
#define BITMAP_SIGNATURE 0x4D42

BMPImage * readBmpImage(char * filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("error opening file");
        return NULL;
    }
    
    struct stat file_stat;
    if (fstat(fd, &file_stat) != 0) { 
        perror("error getting file size");
        close(fd);
        return NULL;
    }
    
    uint8_t * map = (uint8_t*) mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, OFFSET_ZERO);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }

    
    BMPHeader * bmpHeader = (BMPHeader *) map;
    if (bmpHeader->bmp_signature != BITMAP_SIGNATURE) {
        fprintf(stderr, "Invalid BMP signature\n");
        munmap(map, file_stat.st_size);
        close(fd);
        return NULL;
    }

    if (bmpHeader->bits_per_pixel != 8) {
        fprintf(stderr, "Only 8-bit BMP images are supported");
        munmap(map, file_stat.st_size);
        close(fd);
        return NULL;
    }

    BMPImage * bmpImage = malloc(sizeof(BMPImage));
    bmpImage->header = bmpHeader;
    bmpImage->image = map + bmpImage->header->data_offset;
    close(fd);

    return bmpImage; 
}

void freeBmpImage(BMPImage * image) {
    munmap((void *) image->header, image->header->file_size);
    free(image);
}

void dumpBmpImage(BMPImage * bmp, const char * path){
    // Open file for writing
    FILE* file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file for writing\n");
        return;
    }
    printf("Data offset: %d\n", bmp->header->data_offset);
    fwrite(bmp->header, bmp->header->data_offset, 1, file);

    int width = bmp->header->width;
    int height = bmp->header->height;
    int padding = (4 - (width % 4)) % 4;
    uint8_t* data = bmp->image;
    printf("width: %d\n", width);
    printf("height: %d\n", height);
    printf("padding: %d\n", padding);
    int y = 0, x = 0, i = 0;
    for (y = 0; y < height; y++) { 
        for (x = 0; x < width; x++) {
            uint8_t pixel = *(data + y*(width + padding) + x);
            fwrite(&pixel, sizeof(uint8_t), 1, file);
            if(y == height-1) {
                printf("pixel: %d, x:%d\n", pixel, x);
            }
        }
        uint8_t padding_byte = 0x00;
        for (i = 0; i < padding; i++) {
            fwrite(&padding_byte, sizeof(uint8_t), 1, file);
        }
    }

    fclose(file);
}

