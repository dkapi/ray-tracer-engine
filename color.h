#ifndef COLOR_H
#define COLOR_H

#include <vec3.h>

//setting alias for vec3 as color
typedef vec3 color;

void write_color(FILE* out, const color pixel_color)
{
    float r = pixel_color.e[0];
    float g = pixel_color.e[1];
    float b = pixel_color.e[2];

    int r_byte = (int)(255.999 * r);
    int g_byte = (int)(255.999 * g);
    int b_byte = (int)(255.999 * b);

    fprintf(out, "%d %d %d\n", r_byte, g_byte, b_byte);
}




#endif /* COLOR_H */