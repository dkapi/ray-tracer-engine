#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec3.h"
#include "hittable.h"


typedef struct triangle {
    hittable base; // make sure to always have base struct first
    point3 a, b, c;
    vec3 uv_a, uv_b, uv_c;
    material_t* mat;
    aabb_t bbox;
} triangle_t;


triangle_t* create_triangle(const point3* a, const point3* b, const point3* c, material_t* mat);
triangle_t* create_triangle_uv(const point3* a, const point3* b, const point3* c, const vec3* uv_a, const vec3* uv_b, const vec3* uv_c, material_t* mat);
bool hit_triangle(const triangle_t* tri, const ray_t* r, interval_t ray, hit_record_t* rec);
//BVH function prototypes
bool triangle_bounding_box(const triangle_t* tri, aabb_t* output_box);



#endif /* TRIANGLE_H */