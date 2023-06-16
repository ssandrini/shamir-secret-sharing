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
#define SUCCESS 0
#define LSB4 4
#define LSB2 2
#define LSB2_MIN_K 5
#define LSB2_MAX_K 8
#define LSB4_MIN_K 3
#define LSB4_MAX_K 4

static int read_shadow_images(char* dirPath, BMPFile** shadow_images, uint64_t image_size);
static void free_shadow_images(BMPFile** shadows);
int distribute_image(char* image_path,int k, char* dir) {
    BMPFile* secret_file = read_bmp(image_path);
    if (secret_file == NULL) {
        fprintf(stderr, "Failed to read BMP image from file: %s\n", image_path);
        return FAILURE;
    }

    uint64_t image_size = secret_file->image->header->width * secret_file->image->header->height;

    if(image_size % BLOCK_SIZE(k) != 0) {
        fprintf(stderr, "Image size must be divisible by block size, 2k-2\n");
        free_bmp(secret_file);
        return FAILURE;
    }

    BMPFile * shadow_images[MAX_N] = {NULL};
    int n = read_shadow_images(dir, shadow_images, image_size);
    if(n < k || n == -1) {
        free_bmp(secret_file);
        free_shadow_images(shadow_images);
        return FAILURE;
    }

    uint8_t ** shadows = generate_shadows(k, n, image_size, secret_file->image->data);
    if(shadows == NULL) {
        free_bmp(secret_file);
        free_shadow_images(shadow_images);
        return FAILURE;
    }

    if (k >= LSB4_MIN_K && k <= LSB4_MAX_K) {
        for(int i = 0; i < n; i++) {
            lsbHide(shadow_images[i]->image, shadows[i], image_size, LSB4);
            shadow_images[i]->image->header->reserved1 = i;
            if(msync(shadow_images[i]->map, shadow_images[i]->image->header->file_size, MS_SYNC) == -1) {
                fprintf(stderr, "msync failed hidding secret image in shadow image %d\n", i);
                free_bmp(secret_file);
                free_shadows(shadows, n);
                free_shadow_images(shadow_images);
                return FAILURE;
            }
        }
    } else if (k >= LSB2_MIN_K && k <= LSB2_MAX_K) {
        for(int i = 0; i < n; i++) {
            lsbHide(shadow_images[i]->image, shadows[i], image_size, LSB2);
            shadow_images[i]->image->header->reserved1 = i;
            if(msync(shadow_images[i]->map, shadow_images[i]->image->header->file_size, MS_SYNC) == -1) {
                fprintf(stderr, "msync failed hidding secret image in shadow image %d\n", i);
                free_bmp(secret_file);
                free_shadows(shadows, n);
                free_shadow_images(shadow_images);
                return FAILURE;
            }
        }
    } else {
        // esto no deberia pasar nunca porque ya lo chequeamos antes
        fprintf(stderr, "Invalid k value. Please choose a value between 3 and 8.\n");
        return FAILURE;
    }
    
    free_bmp(secret_file);
    free_shadows(shadows, n);
    free_shadow_images(shadow_images);
    return SUCCESS;
}

static int read_shadow_images(char* dirPath, BMPFile** shadow_images, uint64_t image_size) {
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
            BMPFile* file = read_bmp(path);
            if (file != NULL) {
                shadow_images[count++] = file;
            } else if(file->image->header->width * file->image->header->height != image_size) {
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

static void free_shadow_images(BMPFile** shadows) {
    for (int i = 0; i < MAX_N; i++)
        free_bmp(shadows[i]);
}
    