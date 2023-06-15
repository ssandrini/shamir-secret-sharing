#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "bmp.h"
#include "shadow_manager.h"
#include "polynom.h"

int randomGF251() {
    return (rand() % MODULUS) + 1; //Random number [1, 251]
}

//generate shadow function
uint8_t ** generate_shadows(int k, int n, int image_size, uint8_t* secret) {

    if (k < MIN_K || k > MAX_K || image_size % (k - 1) != 0)
        return NULL;

    int block_size = BLOCK_SIZE(k);
    int shadow_size = image_size / (k - 1);

    //Reserve space for shadow matrix, row = shadow
    uint8_t** shadows = (uint8_t**)malloc(sizeof(uint8_t*) * n);

    for(int i = 0; i < n; i++) 
        shadows[i] = (uint8_t*)malloc(sizeof(uint8_t) * shadow_size);
    
    //The dealer divides I into t-non-overlapping 2k − 2-pixel blocks, B1, B2, ..., Bt.

    //For each iteration generate shadow_i byte and shadow_i + 1 byte for each shadow in shadows
    int shadow_i = 0;
    for (int i = 0; i < image_size; i += block_size) {
        int a_0 = secret[i];
        int a_1 = secret[i+1];

        //If mod is 0 then a_i = 1
        a_0 = MODULAR_ARITHMETIC(a_0) ? MODULAR_ARITHMETIC(a_0) : 1;
        a_1 = MODULAR_ARITHMETIC(a_1) ? MODULAR_ARITHMETIC(a_1) : 1;

        Polynom * f = polynom_from_bytes(secret, k);
        uint8_t r_i = MODULAR_ARITHMETIC(randomGF251());

        uint8_t b_0 = MODULAR_ARITHMETIC(-1 * r_i * a_0); //riai,0 + bi,0 = 0
        uint8_t b_1 = MODULAR_ARITHMETIC(-1 * r_i * a_1); //riai,1 + bi,1 = 0

        Polynom * g = polynom_from_bytes(secret + i + k - 2, k); //i + k - 2 for b_0 and b_1
        g->coefficients[0] = b_0;
        g->coefficients[1] = b_1;

        //Save shadow_i and shadow_i + 1 bytes in each shadow
        for (int j = 0; j < n; j++) {
            shadows[j][shadow_i] = evaluate_polynom_mod(f, j + 1);
            shadows[j][shadow_i + 1] = evaluate_polynom_mod(g, j + 1);
        }

        shadow_i += 2;

        polynom_destroy(f);
        polynom_destroy(g);
    }

    return shadows;
}