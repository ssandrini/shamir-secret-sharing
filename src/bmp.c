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
#define MAX_FILE_SIZE 5242880

BMPFile * read_bmp(char * filename) {
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

    if(file_stat.st_size > MAX_FILE_SIZE) {
        fprintf(stderr, "File size too large\n");
        close(fd);
        return NULL;
    }
    
    uint8_t * map = (uint8_t*) mmap(NULL, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, OFFSET_ZERO);
    if (map == MAP_FAILED) {
        fprintf(stderr, "Error mapping file\n");
        close(fd);
        return NULL;
    }

    BMPHeader * bmp_header = (BMPHeader *) map;
    if (bmp_header->bmp_signature != BITMAP_SIGNATURE || bmp_header->bits_per_pixel != 8) {
        fprintf(stderr, "Invalid BMP file\n");
        munmap(map, file_stat.st_size);
        close(fd);
        return NULL;
    }

    BMPFile * bmp_file = malloc(sizeof(BMPFile));
    if (bmp_file == NULL) {
        fprintf(stderr, "Malloc error\n");
        munmap(map, file_stat.st_size);
        close(fd);
        return NULL;
    }

    bmp_file->map = map;
    bmp_file->fd = fd;
    bmp_file->image->header = bmp_header;
    bmp_file->image->data = bmp_file->map + bmp_header->data_offset;
    
    close(fd);

    return bmp_file; 
}

void free_bmp(BMPFile * file) {
    if(file == NULL) {
        return;
    }
    munmap(file->map, file->image->header->file_size);
    close(file->fd); 
    free(file);
}

int dump_bmp_image(BMPImage * bmp, const char * path) {
    FILE* file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file for writing\n");
        return -1;
    }
 
    fwrite(bmp->header, bmp->header->data_offset, 1, file);

    int width = bmp->header->width;
    int height = bmp->header->height;
    int padding = (4 - (width % 4)) % 4;
    uint8_t* data = bmp->data;
    int y = 0, x = 0, i = 0;
    for (y = 0; y < height; y++) { 
        for (x = 0; x < width; x++) {
            uint8_t pixel = *(data + y*(width + padding) + x);
            fwrite(&pixel, sizeof(uint8_t), 1, file);
        }
        uint8_t padding_byte = 0x00;
        for (i = 0; i < padding; i++) {
            fwrite(&padding_byte, sizeof(uint8_t), 1, file);
        }
    }

    fclose(file);
    return 0;
}

