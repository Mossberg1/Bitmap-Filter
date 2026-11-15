#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct 
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint8_t* pixels;
} BitmapImage;

static inline uint8_t* get_pixel(BitmapImage* bitmap, size_t x, size_t y)
{
    return &bitmap->pixels[(y * bitmap->width + x) * 3];
}

BitmapImage* load_bitmap(const char* filepath);
int save_bitmap(const char* filepath, const BitmapImage* bitmap);
void free_bitmap(BitmapImage* bitmap);