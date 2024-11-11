#ifndef RASTER_H
#define RASTER_H

#include <stdio.h>
#include <stdlib.h>

//for raster
typedef struct {
    unsigned char r, g, b;
} pixel_t;

//raster func prototypes
pixel_t **raster_init(int width, int height);
void raster_to_ppm(pixel_t **raster, int width, int height, FILE *img);
void destroy_raster(pixel_t **raster, int height);
#endif /* raster.h */