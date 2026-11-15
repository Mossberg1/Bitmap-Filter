#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"
#include "filter.h"

// TODO: Add bits per pixel and compression check 

int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        printf("Usage: imgf <infile> <outfile>\n");
        return 1;
    }

    char* infile = argv[1];
    char* outfile = argv[2];

    BitmapImage* bitmap = load_bitmap(infile);

    grayscale(bitmap);

    int res = save_bitmap(outfile, bitmap);

    free_bitmap(bitmap);

    if (res != 0) 
    {
        printf("Failed to save output image.\n");
        return 1;
    }

    return 0;
}