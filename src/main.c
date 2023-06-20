#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "image_manager.h"
#include "shadow_manager.h"
#include "polynom.h"

#define ARGS 5

int main(int argc, char *argv[]) {
    
    if (argc != ARGS) {
        fprintf(stderr, "Usage: %s <option> <image> <k> <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char * option = argv[1]; 
    char * image_path = argv[2]; 
    int k = atoi(argv[3]); 
    char * directory = argv[4];

    if(k < MIN_K || k > MAX_K) {
        fprintf(stderr, "k must be between %d and %d\n", MIN_K, MAX_K);
        return EXIT_FAILURE;
    }

    int result = 0;
    if (strcmp(option, "d") == 0) {
        result = distribute_image(image_path, k, directory);
    } else if (strcmp(option, "r") == 0) {
        result = recover_image(image_path, k, directory);
    } else {
        fprintf(stderr, "Invalid option. Please choose 'd' or 'r'.\n");
        return EXIT_FAILURE;
    }

    return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}