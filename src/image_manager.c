#include "bmp.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "image_manager.h"
#include "shadow_manager.h"
#define MAX_FILE_PATH 512
#define FAILURE -1

int distribute_image(char* image_path,int k, char* dir) {
    BMPImage* image = read_bmp_image(image_path);
    if (image == NULL) {
        fprintf(stderr, "Failed to read BMP image from file: %s\n", image_path);
        return FAILURE;
    }

    uint64_t image_size = image->header->width * image->header->height;

    if(image_size % (2*k-2) != 0) {
        fprintf(stderr, "Image size must be divisible by 2k-2\n");
        free_bmp_image(image);
        return FAILURE;
    }

    BMPImage * image_shadows[MAX_N];
    int n = read_shadow_images(dir, image_shadows, image_size);
    if(n < k || n == -1) {
        free_bmp_image(image);
        free_shadow_images(image_shadows);
        return FAILURE;
    }

    uint8_t ** shadows = generate_shadows(k, n, image_size, image->image);
    if(shadows == NULL) {
        free_bmp_image(image);
        free_shadow_images(image_shadows);
        return FAILURE;
    }
    // TODO: replace magic numbers for constants
    if ( k <= 4 && k >= 3) {
        for(int i = 0; i < n; i++) {
            lsbHide(image_shadows[i], shadows[i], image_size, 4);
            image->header->reserved1 = i;
        }
        // TODO: sincronizar memory map
    } else if (k <= 8 && k >= 5) {
        for(int i = 0; i < n; i++) {
            lsbHide(image_shadows[i], shadows[i], image_size, 2);
            image->header->reserved1 = i;
        }
        // TODO: sincronizar memory map
    } else {
        fprintf(stderr, "Invalid k value. Please choose a value between 3 and 8.\n");
        return;
    }
    
    free_bmp_image(image);
    free_shadows(shadows, n);
    free_shadow_images(image_shadows);
}

int read_shadow_images(char* dirPath, BMPImage** shadows, uint64_t image_size) {
    DIR* dir;
    struct dirent* entry;
    struct stat fileStat;

    dir = opendir(dirPath);
    if (dir == NULL) {
        fprintf(stderr, "Could not open directory.\n");
        return FAILURE;
    }

    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        char path[MAX_FILE_PATH];
        snprintf(path, MAX_FILE_PATH, "%s/%s", dirPath, entry->d_name);
        if (stat(path, &fileStat) < 0) {
            fprintf(stderr, "Error reading file stats.\n");
            return FAILURE;
        }
        if (S_ISREG(fileStat.st_mode)) {
            BMPImage* image = read_bmp_image(path);
            if (image != NULL) {
                shadows[count++] = image;
            } else if(image->header->width * image->header->height != image_size) {
                fprintf(stderr, "Image size must be the same for all shadow images.\n");
                return FAILURE;
            } else {
                fprintf(stderr, "Failed to read BMP image from file: %s\n", path);
                return FAILURE;
            }
        }
    }
    closedir(dir);
    return count;
}

void free_shadow_images(BMPImage** shadows) {
    for (int i = 0; i < MAX_N; i++)
        free_bmp_image(shadows[i]);
}
    