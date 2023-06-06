#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "bmp.h"
#include "shadow_manager.h"

int randomGF251() {
    return rand() % 251;
}

//generate shadow function
uint8_t ** generate_shadows(int k, int n, int imageSize, uint8_t* secret) {
    //The dealer divides I into t-non-overlapping 2k - 2-pixel blocks, B1, B2, ..., Bt.
    int blockSize = BLOCK_SIZE(k);
    int blockAmount = (imageSize) / BLOCK_SIZE(k);
    printf("blockAmount: %d\n", blockAmount);
    for (int i = 0; i < blockAmount; i+=blockSize) {
    //    printf("Block %d\n", i);
    }
}