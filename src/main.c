#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

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

  if (argc != 3) {
      fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
      return 1;
  }

  char* pathFrom = argv[1];
  char* pathTo = argv[2];

  // Read the BMP image from the input file
  BMPImage* image = readBmpImage(pathFrom);
  if (image == NULL) {
      fprintf(stderr, "Failed to read BMP image from file: %s\n", pathFrom);
      return 1;
  }

  // Dump the BMP image to the output file
  dumpBmpImage(image, pathTo);

  // Free the allocated memory for the BMP image
  freeBmpImage(image);

  printf("BMP image dumped successfully to file: %s\n", pathTo);

  return 0;

}