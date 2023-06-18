#ifndef __SHADOW_MANAGER_H__
#define __SHADOW_MANAGER_H__

#define MAX_K 8
#define MIN_K 3
#define MAX_N 20
#define BLOCK_SIZE(k) (2*k-2)
#define MASK(n) n == 2 ? 0xFC : 0xF0

uint8_t ** generate_shadows(int k, int n, int image_size, uint8_t* secret);
void free_shadows(uint8_t** shadows, int n);
uint8_t * recover_secret(int k, int image_size, uint8_t** shadows, uint8_t * shadow_numbers);
void hide_secret(BMPImage * shadow_image, uint8_t * shadow, int shadow_size, int bits);
void recover_shadow(BMPImage * shadow_image, uint8_t * shadow, int shadow_size, int bits);
#endif