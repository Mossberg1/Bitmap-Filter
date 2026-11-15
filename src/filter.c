#include "filter.h"

#include <stddef.h>

void grayscale(BitmapImage* bitmap) 
{
    for (size_t y = 0; y < bitmap->height; y++) 
    {
        for (size_t x = 0; x < bitmap->width; x++) 
        {
            uint8_t* pixel = get_pixel(bitmap, x, y);

            uint8_t gray = 0.114 * pixel[0] + 0.587 * pixel[1] + 0.299 * pixel[2];

            pixel[0] = gray;
            pixel[1] = gray;
            pixel[2] = gray;
        }
    }
}