#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "bmp.h"
#include "shadow_manager.h"
#include "polynom.h"

static int randomGF251() {
    return (rand() % MODULUS) + 1; //Random number [1, 251]
}

static bool detect_cheating(int a_0, int a_1, int b_0, int b_1) {
    for (int i = 0; i < MODULUS; i++)
        if (MODULAR_ARITHMETIC(a_0 * i + b_0) == 0 && MODULAR_ARITHMETIC(a_1 * i + b_1) == 0)
            return false;
    return true;
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
            shadows[j][shadow_i] = evaluate_polynom(f, j + 1);
            shadows[j][shadow_i + 1] = evaluate_polynom(g, j + 1);
        }

        shadow_i += 2;

        polynom_destroy(f);
        polynom_destroy(g);
    }

    return shadows;
}


uint8_t * recover_secret(int k, int shadow_size, uint8_t** shadows, int * shadow_numbers) {

    if (k < MIN_K || k > MAX_K)
        return NULL;

    int secret_size = shadow_size * (k - 1);
    int block_size = BLOCK_SIZE(k);
    uint8_t* secret = (uint8_t*)malloc(sizeof(uint8_t) * secret_size);

    int secret_i = 0;
    for (int block_i = 0; block_i < shadow_size; block_i += 2) {
        int m_j[k];
        int d_j[k];

        for (int i = 0; i < k; i++) {
            m_j[i] = shadows[i][block_i];
            d_j[i] = shadows[i][block_i + 1];
        }

        Polynom * f = lagrange_interpolate(m_j, shadow_numbers, k);
        Polynom * g = lagrange_interpolate(d_j, shadow_numbers, k);

        if (detect_cheating(f->coefficients[0], f->coefficients[1], g->coefficients[0], g->coefficients[1])) {
            polynom_destroy(f);
            polynom_destroy(g);
            return NULL;
        }

        for (int i = 0; i < k; i++)
            secret[secret_i * block_size + i] = f->coefficients[i];

        for (int i = 2; i < k; i++)
            secret[secret_i * block_size + i + k - 2] = g->coefficients[i];

        secret_i++;
        polynom_destroy(f);
        polynom_destroy(g);
    }

    return secret;
}