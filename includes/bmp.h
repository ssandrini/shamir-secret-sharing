/*
    Name        	            Size	Offset (Hex)	Description/Notes
    BMP signature	            2 bytes	    0x00	    The characters ‘B’ and ‘M’
    File size	                4 bytes	    0x02	    Size in bytes of the file including headers and pixel data
    Reserved	                4 bytes	    0x06	    Unused
    Data offset	                4 bytes	    0x0A	    Offset in the file where the pixel data is stored		
    Size of the header	        4 bytes	    0x0E	    The header is fixed size: 40 bytes
    Width	                    4 bytes	    0x12	    Width of the image in pixels
    Height	                    4 bytes	    0x16	    Height of the image in pixels
    Planes	                    2 bytes	    0x1A	    Number of color planes (must be 1)
    Bits per pixel	            2 bytes	    0x1C	    Number of bits per pixel
    Compression	                4 bytes	    0x1E	    Compression method
    Image size	                4 bytes	    0x22	    Can be 0 if image is not compressed, otherwise is the size of the compressed image
    Pixels per meter in X axis	4 bytes	    0x26	    Horizontal resolution in pixels per meter
    Pixels per meter in Y axis	4 bytes	    0x2A	    Vertical resolution in pixels per meter
    Colors used	                4 bytes	    0x2E	    Number used colors
    Important colors	        4 bytes	    0x32	    Number of important color. Can be 0 If all colors are important
*/

#ifndef __BMP_H__
#define __BMP_H__

#include <stdint.h>

typedef struct  __attribute__((__packed__)) BMPHeader {
    uint16_t bmp_signature;         // (must be 0x4D42 for BMP files)
    uint32_t file_size;             
    uint32_t reserved;              // reserved; must be 0
    uint32_t data_offset;           
    uint32_t header_size;           
    uint32_t width;                 
    uint32_t height;                
    uint16_t color_planes;          
    uint16_t bits_per_pixel;        // number of bits per pixel, must be 8
    uint32_t compression_method;    // (0 = none, 1 = RLE-8, 2 = RLE-4). Must be 0
    uint32_t image_size;            
    uint32_t x_pixels_per_meter;    
    uint32_t y_pixels_per_meter;
    uint32_t num_colors;            
    uint32_t important_colors;      
} BMPHeader;

typedef struct BMPImage {
    BMPHeader * header; 
    uint8_t * image;
} BMPImage;

BMPImage * readBmpImage(char * filename);
void freeBmpImage(BMPImage * image);
void dumpBmpImage(BMPImage * image, const char * outputPath);


#endif // __BMP_H__