#ifndef TEXTURE_H
#define TEXTURE_H

#include "../vec3.h"
#include "../color.h"
#include "../perlin.h"

//forward declarations
typedef struct texture texture_t;
typedef color (*texture_value_fn)(const texture_t *self, double u, double v, const point3 *p);

typedef struct texture {
    texture_value_fn value;
}texture_t;

typedef struct solid_color {
    texture_t base;
    color albedo;
}solid_color_t;

// checker texture struct
typedef struct checker_texture {
    texture_t base;             // base interface
    double inv_scale;           // inverse scale for the checker pattern
    texture_t *even;            // pointer to the "even" texture
    texture_t *odd;             // pointer to the "odd" texture
} checker_texture_t;

// image texture struct
typedef struct {
    texture_t base;      // base interface
    unsigned char* data; // image data (RGB or RGBA)
    int width, height;   // dimensions of the image
    int bytes_per_pixel; // number of channels per pixel (e.g., 3 for RGB, 4 for RGBA)
} image_texture_t;

// perlin noise texture struct
typedef struct noise_texture {
    texture_t base;
    perlin_t* noise;
    double scale;
} noise_texture_t;

//solid color 
color solid_color_value(const texture_t *self, double u, double v, const point3* p);
solid_color_t* create_solid_color(double red, double green, double blue);
// checker texture functions
color checker_texture_value(const texture_t *self, double u, double v, const point3 *p);
checker_texture_t* create_checker_texture(double scale, texture_t *even, texture_t *odd);
checker_texture_t* create_checker_texture_solid(double scale, color c1, color c2);
//image textures
image_texture_t* create_image_texture(const char* filename);
void free_image_texture(image_texture_t* tex);
color image_texture_value(const texture_t* self, double u, double v, const point3* p);
//perlin noise textures
noise_texture_t* create_noise_texture(double scale);
void free_noise_texture(noise_texture_t* noise_tex);


#endif /* TEXTURE_H */