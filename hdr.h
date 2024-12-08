#ifndef HDR_H
#define HDR_H

#include "vec3.h"
#include "color.h"
#include "textures/stb_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


typedef struct {
    float* data;     // HDR pixel data
    int width;       
    int height;      
    int channels;    // color channels for rgb
} hdr_texture_t;

hdr_texture_t* load_hdr_image(const char* filepath);
void free_hdr_texture(hdr_texture_t* hdr_tex);
color sample_hdr(const hdr_texture_t* hdr_tex, double u, double v);
void direction_to_uv(const vec3* dir, double* u, double* v);

#endif // HDR_H