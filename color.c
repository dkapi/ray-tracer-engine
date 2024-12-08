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
    color aces_tone = aces_tone_map(&pixel_color, 0.05);

    pixel->r = (unsigned char)(256 * interval_clamp(&intensity, aces_tone.x));
    pixel->g = (unsigned char)(256 * interval_clamp(&intensity, aces_tone.y));
    pixel->b = (unsigned char)(256 * interval_clamp(&intensity, aces_tone.z));
}

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