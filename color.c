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

    pixel->r = (unsigned char)(256 * interval_clamp(&intensity, linear_to_gamma(pixel_color.x)));
    pixel->g = (unsigned char)(256 * interval_clamp(&intensity, linear_to_gamma(pixel_color.y)));
    pixel->b = (unsigned char)(256 * interval_clamp(&intensity, linear_to_gamma(pixel_color.z)));
}