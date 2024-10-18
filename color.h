#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

//setting alias for vec3 as color
typedef vec3 color;

void write_color(FILE* out, const color pixel_color);

#endif /* COLOR_H */