#include "color.h"

static const interval_t intensity = {.min = 0.000, .max = 0.999};

static inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0) {
        return sqrt(linear_component);
    }
    return 0;
}

void write_color(pixel_t *pixel, const color pixel_color) {
    color hdri_tone = tone_map(&pixel_color, 1.1); // use later for hdri's

    pixel->r = (unsigned char)(256 * interval_clamp(&intensity, linear_to_gamma(pixel_color.x)));
    pixel->g = (unsigned char)(256 * interval_clamp(&intensity, linear_to_gamma(pixel_color.y)));
    pixel->b = (unsigned char)(256 * interval_clamp(&intensity, linear_to_gamma(pixel_color.z)));
}

color tone_map(const color* hdr_color, double exposure) {
    return vec3_create_values(
        1.0 - exp(-hdr_color->x * exposure),
        1.0 - exp(-hdr_color->y * exposure),
        1.0 - exp(-hdr_color->z * exposure)
    );
}