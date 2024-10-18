#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "stdlib.h"
#include "color.h"
#include "hittable.h"


typedef struct {
    double aspect_ratio;
    int image_width;
    int image_height;
    double viewport_height;
    double viewport_width;
    double focal_length;
    int samples_per_pixel;
    double pixel_samples_scale; // scale for color based on samples
    point3 camera_center;
    vec3 viewport_u;
    vec3 viewport_v;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 pixel_00_loc;
} camera_t;

void camera_initialize(camera_t* c);
void render(const camera_t* camera, hittable_list* world, FILE* img);
color ray_color(const ray_t *r, const hittable_list *world);
ray_t get_ray(const camera_t* camera, int i, int j);
vec3 sample_square();



#endif /* CAMERA_H */