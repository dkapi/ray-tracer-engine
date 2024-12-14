#include  "texture.h"
//3rd party library 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    ct->base.value = (texture_value_fn)checker_texture_value;
    ct->inv_scale = 1.0 / scale;
    ct->even = even;
    ct->odd = odd;
    return ct;
}

checker_texture_t* create_checker_texture_solid(double scale, color c1, color c2) {
    texture_t* even = (texture_t*)create_solid_color(c1.x, c1.y, c1.z);
    texture_t* odd  = (texture_t*)create_solid_color(c2.x, c2.y, c2.z);

    checker_texture_t* checker = (checker_texture_t*)malloc(sizeof(checker_texture_t));
    checker->base.value = checker_texture_value;
    checker->inv_scale = 1.0 / scale;
    checker->even = even;
    checker->odd = odd;

    return checker;
}

//image textures
image_texture_t* create_image_texture(const char* filename) {
    image_texture_t* tex = (image_texture_t*)malloc(sizeof(image_texture_t));

    // load image data using stb_image
    tex->data = stbi_load(filename, &tex->width, &tex->height, &tex->bytes_per_pixel, 0);
    if (!tex->data) {
        fprintf(stderr, "Failed to load texture image: %s\n", filename);
        free(tex);
        return NULL;
    }

    tex->base.value = (texture_value_fn)image_texture_value;
    return tex;
}
void free_image_texture(image_texture_t* tex) {
    if (tex->data) {
        stbi_image_free(tex->data); 
    }
    free(tex);
}

color image_texture_value(const texture_t* self, double u, double v, const point3* p) {
    const image_texture_t* tex = (const image_texture_t*)self;

    // clamp UV coordinates to [0, 1]
    u = u < 0 ? 0 : (u > 1 ? 1 : u);
    v = v < 0 ? 0 : (v > 1 ? 1 : v);

    // convert UV coordinates to pixel indices
    int i = (int)(u * tex->width);
    int j = (int)((1 - v) * tex->height - 0.001);
    if (j < 0) j = 0;
    if (j >= tex->height) j = tex->height - 1;

    const int index = j * tex->width * tex->bytes_per_pixel + i * tex->bytes_per_pixel;

    //convert pixel  data to [0,1]
    double r = tex->data[index] / 255.0;
    double g = tex->data[index + 1] / 255.0;
    double b = tex->data[index + 2] / 255.0;

    return vec3_create_values(r, g, b);
}

// perlin noise textures
// can change  type of perlin effect here, right now sin turbulence, so like marbling effect
color noise_texture_value(const texture_t* self, double u, double v, const point3* p) {
    const noise_texture_t* noise_tex = (const noise_texture_t*)self;

    // Perlin noise with turbulence for the marble effect
    double marble_pattern = sin(noise_tex->scale * p->z + 10 * perlin_turbulence(noise_tex->noise, p, 7));

    // normalize to [0, 1] and scale color
    double noise_val = 0.5 * (1.0 + marble_pattern);

    return vec3_create_values(noise_val, noise_val, noise_val);
}


noise_texture_t* create_noise_texture(double scale) {
    noise_texture_t* noise_tex = (noise_texture_t*)malloc(sizeof(noise_texture_t));

    noise_tex->base.value = (texture_value_fn)noise_texture_value; 
    noise_tex->noise = perlin_create(); 
    noise_tex->scale = scale; 

    return noise_tex;
}

void free_noise_texture(noise_texture_t* noise_tex) {
    if (!noise_tex) return;
    perlin_free(noise_tex->noise);
    free(noise_tex);
}


