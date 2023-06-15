#ifndef __SHADOW_MANAGER_H__
#define __SHADOW_MANAGER_H__

#define MAX_K 8
#define MIN_K 3
#define BLOCK_SIZE(k) (2*k-2)

void distribute_image(char* image_path, int k, char* dir);

uint8_t ** generate_shadows(int k, int n, int image_size, uint8_t* secret);

#endif