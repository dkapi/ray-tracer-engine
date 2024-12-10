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
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    if (r != r) r = 0.0; // Check if r is NaN
    if (g != g) g = 0.0; // Check if g is NaN
    if (b != b) b = 0.0; // Check if b is NaN

    // applying tone mapping here, if color looks off we can mess with exposure lvl
    color aces_tone = aces_tone_map(&(color){r, g, b}, 0.1);

    pixel->r = (unsigned char)(256 * interval_clamp(&intensity, aces_tone.x));
    pixel->g = (unsigned char)(256 * interval_clamp(&intensity, aces_tone.y));
    pixel->b = (unsigned char)(256 * interval_clamp(&intensity, aces_tone.z));
}

/************************
 * below are different types of tone mapping
 */

color tone_map(const color* hdr_color, double exposure) {
    return vec3_create_values(
        1.0 - exp(-hdr_color->x * exposure),
        1.0 - exp(-hdr_color->y * exposure),
        1.0 - exp(-hdr_color->z * exposure)
    );
}

color aces_tone_map(const color* hdr_color, double exposure) {
    color scaled = vec3_multiply_by_scalar(hdr_color, exposure);

    return vec3_create_values(
        scaled.x / (scaled.x + 0.024),
        scaled.y / (scaled.y + 0.024),
        scaled.z / (scaled.z + 0.024)
    );
}

color tone_map_reinhard(const color* hdr_color) {
    return vec3_divide_by_scalar(hdr_color, (1.0 + vec3_length(hdr_color)));
}