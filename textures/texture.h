#ifndef TEXTURE_H
#define TEXTURE_H

#include "../vec3.h"
#include "../color.h"

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

// checker texture
typedef struct checker_texture {
    texture_t base;             // base "class"
    double inv_scale;           // inverse scale for the checker pattern
    texture_t *even;            // pointer to the "even" texture
    texture_t *odd;             // pointer to the "odd" texture
} checker_texture_t;

//solid color 
color solid_color_value(const texture_t *self, double u, double v, const point3* p);
solid_color_t* create_solid_color(double red, double green, double blue);
// checker texture functions
color checker_texture_value(const texture_t *self, double u, double v, const point3 *p);
checker_texture_t* create_checker_texture(double scale, texture_t *even, texture_t *odd);
checker_texture_t* create_checker_texture_solid(double scale, color c1, color c2);

#endif /* TEXTURE_H */