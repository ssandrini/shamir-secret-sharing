#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "bmp.h"
#include "shadow_manager.h"
#include "polynom.h"

static int randomGF251() {
    return MODULAR_ARITHMETIC(1 + rand() % (MODULUS - 1)); //Random number [1, 250]
}

static bool detect_cheating(int a_0, int a_1, int b_0, int b_1) {
    for (int i = 1; i < MODULUS; i++)
        if (MODULAR_ARITHMETIC(a_0 * i + b_0) == 0 && MODULAR_ARITHMETIC(a_1 * i + b_1) == 0)
            return false;
    return true;
}

uint8_t ** generate_shadows(int k, int n, int image_size, uint8_t* secret) {
    int block_size = BLOCK_SIZE(k);
    int shadow_size = image_size / (k - 1);

    // Reserve space for shadow matrix, row = shadow
    uint8_t** shadows = (uint8_t**)malloc(sizeof(uint8_t*) * n);
    if(shadows == NULL) {
        fprintf(stderr, "Error allocating memory for shadows\n");
        return NULL;
    }

    for(int i = 0; i < n; i++) {
        shadows[i] = (uint8_t*)malloc(sizeof(uint8_t) * shadow_size);
        if(shadows[i] == NULL) {
            free_shadows(shadows, i);
            fprintf(stderr, "Error allocating memory for shadows\n");
            return NULL;
        }
    }
    // The dealer divides I into t-non-overlapping 2k − 2-pixel blocks, B1, B2, ..., Bt.
    // For each iteration generate shadow_i byte and shadow_i + 1 byte for each shadow in shadows

    for (int shadow_i = 0, i = 0; i < image_size; i += block_size, shadow_i += 2) {
        uint8_t a_0 = secret[i];
        uint8_t a_1 = secret[i+1];

        //If mod is 0 then a_i = 1
        a_0 = MODULAR_ARITHMETIC(a_0) > 0 ? a_0 : 1;
        a_1 = MODULAR_ARITHMETIC(a_1) > 0 ? a_1 : 1;

        Polynom * f = create_polynom(secret + i, k);
        if(f == NULL) {
            fprintf(stderr, "Error allocating memory for polynom\n");
            free_shadows(shadows, n);
            return NULL;
        }

        uint8_t r_i = randomGF251();

        uint8_t b_0 = MODULAR_ARITHMETIC(-1 * r_i * a_0); // riai,0 + bi,0 = 0
        uint8_t b_1 = MODULAR_ARITHMETIC(-1 * r_i * a_1); // riai,1 + bi,1 = 0

        Polynom * g = create_polynom(secret + i + k - 2, k); // i + k - 2 for b_0 and b_1
        if(g == NULL) {
            fprintf(stderr, "Error allocating memory for polynom\n");
            polynom_destroy(f);
            free_shadows(shadows, n);
            return NULL;
        }

        g->coefficients[0] = b_0;
        g->coefficients[1] = b_1;

        //Save shadow_i and shadow_i + 1 bytes in each shadow
        for (int j = 0; j < n; j++) {
            shadows[j][shadow_i] = evaluate_polynom(f, j + 1);
            shadows[j][shadow_i + 1] = evaluate_polynom(g, j + 1);
        }

        polynom_destroy(f);
        polynom_destroy(g);
    }

    return shadows;
}

uint8_t * recover_secret(int k, int shadow_size, uint8_t** shadows, uint8_t * shadow_numbers) {
    int secret_size = shadow_size * (k - 1);
    int block_size = BLOCK_SIZE(k);
    uint8_t* secret = (uint8_t*) malloc(sizeof(uint8_t) * secret_size);
    if(secret == NULL) {
        fprintf(stderr, "Error allocating memory for secret\n");
        return NULL;
    }

    for (int block_i = 0, secret_i = 0; block_i < shadow_size ; block_i += 2, secret_i++) {
        uint8_t m_j[k];
        uint8_t d_j[k];

        for (int i = 0; i < k; i++) {
            m_j[i] = shadows[i][block_i];
            d_j[i] = shadows[i][block_i + 1];
        }

        Polynom * f = lagrange_interpolate(shadow_numbers, m_j, k);
        Polynom * g = lagrange_interpolate(shadow_numbers, d_j, k);
        if(f == NULL || g == NULL) {
            fprintf(stderr, "Error allocating memory for polynom\n");
            polynom_destroy(f);
            polynom_destroy(g);
            free(secret);
            return NULL;
        }

        if (detect_cheating(f->coefficients[0], f->coefficients[1], g->coefficients[0], g->coefficients[1])) {
            polynom_destroy(f);
            polynom_destroy(g);
            free(secret);
            fprintf(stderr, "Cheating detected\n");
            return NULL;
        }

        for (int i = 0; i < k; i++)
            secret[secret_i * block_size + i] = f->coefficients[i];

        for (int i = 2; i < k; i++)
            secret[secret_i * block_size + i + k - 2] = g->coefficients[i];

        polynom_destroy(f);
        polynom_destroy(g);
    }
    return secret;
}

void free_shadows(uint8_t** shadows, int n) {
    for(int i = 0; i < n; i++) {
        free(shadows[i]);
    }
    free(shadows);
}

void hide_shadow(BMPImage * shadow_image, uint8_t * shadow, int shadow_size, int bits) {
    int k = 0;
    for(int i = 0; i < shadow_size; i++) {
        for(int j = 0; j < 8/bits; j++) {
            uint8_t pixel = (shadow_image->data[k] >> bits ) << bits;
            uint8_t bits_to_hide = shadow[i] >> (8 - bits);
            pixel = pixel | bits_to_hide;
            shadow_image->data[k++] = pixel;
            shadow[i] = shadow[i] << bits;
        }   
    }
}

void recover_shadow(BMPImage * shadow_image, uint8_t * shadow, int shadow_size, int bits) {
    int k = 0;
    for(int i = 0; i < shadow_size; i++) {
        shadow[i] = 0;
        for(int j = 0; j < 8/bits; j++) {
            uint8_t pixel = shadow_image->data[k++];
            uint8_t recovered_bits = pixel << (8 - bits);
            recovered_bits = recovered_bits >> (8 - bits);
            shadow[i] = (shadow[i] << bits) | recovered_bits;
        }
    }
}