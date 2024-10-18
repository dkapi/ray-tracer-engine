#include "color.h"

static const interval_t intensity = {.min = 0.000, .max = 0.999};

void write_color(FILE* out, const color pixel_color)
{
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    int r_byte = (int)(256 * interval_clamp(&intensity, r));
    int g_byte = (int)(256 * interval_clamp(&intensity, g));
    int b_byte = (int)(256 * interval_clamp(&intensity, b));


    fprintf(out, "%d %d %d\n", r_byte, g_byte, b_byte);
}