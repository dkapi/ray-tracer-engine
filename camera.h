#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "stdlib.h"
#include "color.h"
#include "hittable.h"
#include "bvh.h"
#include "raster.h"


typedef struct {
    int image_width;
    int image_height;
    int max_depth;             // for recursive call in ray_color
    int samples_per_pixel;
    double aspect_ratio;
    double viewport_height;
    double viewport_width;
    double focal_length;
    double vfov;                // vertical field of view angle
    double pixel_samples_scale; // scale for color based on samples
    double defocus_angle;
    double focus_dist;
    point3 lookfrom;
    point3 lookat;
    point3 camera_center;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;
    vec3 vup;
    vec3 u, v, w;
    vec3 viewport_u;
    vec3 viewport_v;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 pixel_00_loc;
    color background;
} camera_t;



void camera_initialize(camera_t* c);
void render(const camera_t* camera, bvh_node_t* world, pixel_t** raster);
color ray_color(const ray_t *r, int depth, const bvh_node_t* world, const color* background);
ray_t get_ray(const camera_t* camera, int i, int j);
vec3 sample_square();

#endif /* CAMERA_H */