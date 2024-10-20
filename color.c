#include "color.h"

static const interval_t intensity = {.min = 0.000, .max = 0.999};

void write_color(FILE* out, const color pixel_color) {
    unsigned char r_byte = (unsigned char)(256 * interval_clamp(&intensity, pixel_color.x));
    unsigned char g_byte = (unsigned char)(256 * interval_clamp(&intensity, pixel_color.y));
    unsigned char b_byte = (unsigned char)(256 * interval_clamp(&intensity, pixel_color.z));

    fwrite(&r_byte, 1, 1, out);
    fwrite(&g_byte, 1, 1, out);
    fwrite(&b_byte, 1, 1, out);
}