#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "stdlib.h"
#include "color.h"
#include "hittable.h"

#define ASPECT_RATIO  16.0 / 9.0
#define I_WIDTH 1024
#define I_HEIGHT (int) (I_WIDTH / (ASPECT_RATIO))
#define FOCAL_LENGHT 1.0
#define VIEWPORT_HEIGHT 2.0




typedef struct {
    double aspect_ratio;
    int image_width;
    int image_height;
    double focal_length;
    double viewport_height;
    double viewport_width;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 camera_center;
    vec3 viewport_u;
    vec3 viewport_v;
    vec3 pixel_00_loc;
}camera_t;


camera_t* camera_initialize();
void render(const camera_t* camera, hittable_list* world, FILE* img);
color ray_color(const ray_t *r, const hittable_list *world);



#endif /* CAMERA_H */