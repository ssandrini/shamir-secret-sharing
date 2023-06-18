#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "image_manager.h"
#include "shadow_manager.h"
#include "polynom.h"

#define ARGS 5

// TODO CARPETA BIN NO SE CREA, HACE QUE NO COMPILE

int main(int argc, char *argv[]) {
    // TODO: Revisar no tener printfs adicionales, en ese caso pasarlos a --verbose
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


    
    // BMPImage image;
    // image.data = malloc(8);
    // image.data[0] = 0b00000001;
    // image.data[1] = 0b00000010;
    // image.data[2] = 0b00000011;
    // image.data[3] = 0b00000100;
    // image.data[4] = 0b00000101;
    // image.data[5] = 0b00000110;
    // image.data[6] = 0b00000111;
    // image.data[7] = 0b00001000;

    // uint8_t shadow[4] = { 0b10010001, 0b01100100, 0b10111110, 0b00000000 };
    // hide_secret(&image, shadow, 4, 4);
    // for(int i = 0; i < 8; i++) {
    //     printf("%02x\n", image.data[i]);
    // }
    // printf("----------------------\n");
    // recover_shadow(&image, shadow, 4, 4);
    // for(int i = 0; i < 8; i++) {
    //     printf("%02x\n", image.data[i]);
    // }

    // free(image.data);
    // return 0;

    // Polynom * polynom = lagrange_interpolate((uint8_t[]){1, 2, 3, 4}, (uint8_t[]){163, 111, 84, 70}, 4);
    // for(int i = 0; i < polynom->degree + 1; i++) {
    //     printf("%d ", polynom->coefficients[i]);
    // }
    // polynom_destroy(polynom);
    // return 0;
    // 1 + 14x + 150x^2 + 249x^3
}