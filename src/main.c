#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "image_manager.h"

int main(int argc, char* argv[]) {
  // if (argc < 2 || strcmp("h", argv[1]) == 0) {
  //   printf("Usage: %s <option> <image> <k> <directory>\n", argv[0]);
  //   return 0;
  // }

  // char * option = argv[1]; // 'd'or 'r'
  // char * image = argv[2]; // image file name
  // int k = atoi(argv[3]); // number of clusters
  // char * directory = argv[4]; // directory to save the result

  // if (strcmp(option, "d") == 0) {
  //     // TODO: Implement the distribution logic
  // } else if (strcmp(option, "r") == 0) {
  //     // TODO: Implement the recovery logic
  // } else {
  //     printf("Invalid option. Please choose 'd' or 'r'.\n");
  // }

  // return 0;

  if (argc != 5) {
      fprintf(stderr, "Uso incorrecto: %s [d|r] [imagen] [k] [directorio]\n", argv[0]);
      return 1;
  }

  char *option = argv[1];
  char *imagePath = argv[2];
  int k = atoi(argv[3]);
  char *dir = argv[4];

  // // Read the BMP image from the input file //TODO: REMOVE LATER
  // BMPImage* image = readBmpImage(imagePath);
  // if (image == NULL) {
  //     fprintf(stderr, "Failed to read BMP image from file: %s\n", imagePath);
  //     return 1;
  // }

  if (option[0] == 'd') {
      distribute_image(imagePath, k, dir);
      printf("distribuir\n");
  } else if (option[0] == 'r') {
      // recover_image(image, k, dir);
      printf("recuperar\n");
  } else {
      fprintf(stderr, "Opción no reconocida: %s. Las opciones son 'd' para distribuir, 'r' para recuperar.\n", option);
      return 1;
  }

  // // Dump the BMP image to the output file
  // dumpBmpImage(image, dir);

  // // Free the allocated memory for the BMP image //TODO: REMOVE LATER
  // freeBmpImage(image);

  printf("BMP image dumped successfully to file: %s\n", imagePath);

  return 0;

}