#include "raster.h"

pixel_t** raster_init(int width, int height) {
        pixel_t** raster = malloc(height * sizeof(pixel_t *));
        
        for(int i = 0; i < height; ++i) {
            raster[i] = malloc(width * sizeof(pixel_t));
        }
    return raster;
}

void raster_to_ppm(pixel_t **raster, int width, int height, FILE *img) {
    fprintf(img, "P6\n%d %d\n255\n", width, height);
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            fwrite(&raster[y][x], 1, 3, img);
        }
    }
}

void destroy_raster(pixel_t** raster, int height) {
    for(int i = 0; i < height; ++i) {
        free(raster[i]);
    }
    free(raster);
}