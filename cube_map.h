#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include "hittable.h"
#include "textures/stb_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * ordering for 6 textures for cubemap!!
 * RIGHT
 * LEFT
 * TOP
 * BOTTOM
 * FRONT
 * BACK
 */

// cube map single image layout types
#define CUBEMAP_LAYOUT_CROSS 0
#define CUBEMAP_LAYOUT_HORIZONTAL 1

// cube map struct
typedef struct {
    texture_t* faces[6];
} cubemap_t;

// functions
cubemap_t* create_cubemap(const char* file_paths[6]);
cubemap_t* create_cubemap_from_single_image(const char* filepath, int layout);
color cubemap_value(const cubemap_t* cubemap, const vec3* direction);
void destroy_cubemap(cubemap_t* cubemap);

#endif /* CUBE_MAP_H */
