#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"


typedef struct {
    hittable base;
    point3 Q;
    vec3 w,u,v;
    vec3 normal;
    double D;
    material_t* mat;
    aabb_t bbox;
}quads_t;


quads_t* quad_create(const point3* Q, const vec3* u, const vec3* v, material_t* mat);
hittable* create_box(const point3* min, const point3* max, material_t* mat);
hittable* create_textured_box(const point3* min, const point3* max, material_t* top_mat, material_t* side_mat, material_t* bottom_mat);
bool hit_quad(const quads_t* tri, const ray_t* r, interval_t ray, hit_record_t* rec);
bool quad_is_interior(double alpha, double beta, hit_record_t* rec);
bool quad_bounding_box(const quads_t* quad, aabb_t* output_box);
#endif /* QUAD_H */