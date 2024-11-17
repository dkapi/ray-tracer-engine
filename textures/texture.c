#include  "texture.h"

// solid color value function
color solid_color_value(const texture_t *self, double u, double v, const point3 *p) {
    const solid_color_t *sc = (const solid_color_t *)self;
    return sc->albedo;
}

// init solid color
solid_color_t* create_solid_color(double red, double green, double blue) {
    solid_color_t* sc = (solid_color_t*)malloc(sizeof(solid_color_t));
    sc->base.value = solid_color_value; // Function pointer
    sc->albedo = (color){red, green, blue};
    return sc;
}

//checker texture
color checker_texture_value(const texture_t *self, double u, double v, const point3 *p) {
    const checker_texture_t *ct = (const checker_texture_t*) self;

    int xInt = (int)floor(ct->inv_scale * p->x);
    int yInt = (int)floor(ct->inv_scale * p->y);
    int zInt = (int)floor(ct->inv_scale * p->z);

    bool  is_even = (xInt + yInt + zInt) % 2 == 0;

    return is_even ? ct->even->value(ct->even, u, v, p) : ct->odd->value(ct->odd, u, v, p);
}

checker_texture_t* create_checker_texture(double scale, texture_t *even, texture_t *odd) {
     checker_texture_t* ct = (checker_texture_t*)malloc(sizeof(checker_texture_t));
    ct->base.value = (texture_value_fn)checker_texture_value; // function pointer assignment
    ct->inv_scale = 1.0 / scale;
    ct->even = even;
    ct->odd = odd;
    return ct;
}

checker_texture_t* create_checker_texture_solid(double scale, color c1, color c2) {
    texture_t* even = (texture_t*)create_solid_color(c1.x, c1.y, c1.z);
    texture_t* odd  = (texture_t*)create_solid_color(c2.x, c2.y, c2.z);

    checker_texture_t* checker = (checker_texture_t*)malloc(sizeof(checker_texture_t));
    checker->base.value = checker_texture_value; // Assign function pointer
    checker->inv_scale = 1.0 / scale;
    checker->even = even;
    checker->odd = odd;

    return checker;
}
