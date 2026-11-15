#include "bitmap.h"

#include <stdio.h>
#include <stdlib.h>

static uint8_t* read_pixels_24(FILE* img, uint32_t width, uint32_t height) 
{
    uint8_t* buffer = malloc((size_t)width*height*3);
    if (buffer == NULL) 
    {
        return NULL;
    }

    // Point file cursor to the starts of the pixels.
    uint32_t pixels_start = 0;
    fseek(img, 10, SEEK_SET);
    fread(&pixels_start, 4, 1, img);
    fseek(img, pixels_start, SEEK_SET);

    uint32_t padding = (4 - (width * 3) % 4) % 4;
    size_t row_size = (size_t)width * 3;

    // Read pixels into buffer.
    for (size_t row = 0; row < height; row++) 
    {
        // Read row into buffer.
        fread(&buffer[row * row_size], 1, row_size, img);

        // Ignore padding.
        fseek(img, padding, SEEK_CUR);
    }

    return buffer;
}

static uint8_t* read_pixels(FILE* img, uint32_t width, uint32_t height) 
{
    return read_pixels_24(img, width, height);
}

void free_bitmap(BitmapImage* bitmap)
{
    if (bitmap == NULL) 
    {
        return;
    }

    free(bitmap->pixels);
    free(bitmap);
}

BitmapImage* load_bitmap(const char* filepath) 
{
    FILE* img = fopen(filepath, "rb");
    if (img == NULL) 
    {
        printf("Could not open image: %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    BitmapImage* bitmap = malloc(sizeof(BitmapImage));
    if (bitmap == NULL) 
    {
        printf("Could not allocate memory for bitmap image struct.\n");
        goto fail;
    }
    bitmap->pixels = NULL;

    // Read the size of the image.
    uint32_t size = 0;
    fseek(img, 2, SEEK_SET);
    fread(&size, 4, 1, img);

    bitmap->size = size;

    // Read the header size.
    uint32_t header_size = 0;
    fseek(img, 14, SEEK_SET);
    fread(&header_size, 4, 1, img);

    // Read width and height of image.
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t dimension_bytes = 0; // Number of bytes for width and height.
    
    if (header_size == 12) // Old header.
    {
        dimension_bytes = 2;
    }
    else if (header_size >= 40) // New header. 
    {
        dimension_bytes = 4;
    }
    else // Invalid header.
    {
        printf("Invalid header size.\n");
        goto fail;
    }

    fseek(img, 18, SEEK_SET);
    fread(&width, dimension_bytes, 1, img);
    fread(&height, dimension_bytes, 1, img);

    bitmap->width = width;
    bitmap->height = height;

    // Add bits per pixel and compression check later.
    uint8_t* pixels = read_pixels(img, width, height);
    if (pixels == NULL) 
    {
        printf("Could not allocate pixel buffer.\n");
        goto fail;
    }

    bitmap->pixels = pixels;

    fclose(img);

    return bitmap;

fail:
    free_bitmap(bitmap);
    fclose(img);
    exit(EXIT_FAILURE);
}

static void write_u16(FILE* f, uint16_t val) 
{
    fwrite(&val, 2, 1, f);
}

static void write_u32(FILE* f, uint32_t val) 
{
    fwrite(&val, 4, 1, f);
}

int save_bitmap(const char* filepath, const BitmapImage* bitmap) 
{
    FILE* f = fopen(filepath, "wb");
    if (f == NULL) 
    {
        return 1;
    }

    uint32_t padding    = (4 - (bitmap->width * 3) % 4) % 4;
    uint32_t image_size = (bitmap->width * 3 + padding) * bitmap->height;

    // File header (14 bytes)
    fwrite("BM", 1, 2, f);
    write_u32(f, 54 + image_size); // file size
    write_u32(f, 0);               // reserved
    write_u32(f, 54);              // pixel data offset

    // Info header (40 bytes)
    write_u32(f, 40);              // header size
    write_u32(f, bitmap->width);
    write_u32(f, bitmap->height);
    write_u16(f, 1);               // planes
    write_u16(f, 24);              // bits per pixel
    write_u32(f, 0);               // compression
    write_u32(f, image_size);
    write_u32(f, 2835);            // horizontal resolution (72 DPI)
    write_u32(f, 2835);            // vertical resolution
    write_u32(f, 0);               // colors used
    write_u32(f, 0);               // important colors

    // Zeros is the padding for each row.
    uint8_t zeros[3] = {0, 0, 0};
    size_t row_size = (size_t)bitmap->width * 3;

    // Write pixels to outfile.
    for (size_t row = 0; row < bitmap->height; row++)
    {
        fwrite(&bitmap->pixels[row * row_size], 1, row_size, f);
        fwrite(zeros, 1, padding, f);
    }

    fclose(f);

    return 0;
}