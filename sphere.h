#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

// sphere structure 
typedef struct {
    hittable base;     // make sure to always have base first
    point3 center;
    double radius;
    material_t* mat;
    aabb_t bbox;
} sphere_t;

typedef struct {
    hittable base;
    point3 center1;
    point3 center2; 
    double radius;
    double time_start;
    double time_end;
    material_t* mat;
    aabb_t bbox;
} moving_sphere_t;

//static sphere 
sphere_t* sphere_create(const point3 *center, double radius, material_t* mat);
bool hit_sphere(const sphere_t *s, const ray_t *r, interval_t ray, hit_record_t *rec);
bool sphere_bounding_box(const sphere_t* s, aabb_t *output_box);

//moving sphere
moving_sphere_t* moving_sphere_create(const point3* c1, const point3* c2, double radius,
                                       double start_time, double end_time, material_t* mat);
bool hit_moving_sphere(const moving_sphere_t* ms, const ray_t* r, interval_t ray, hit_record_t *rec);
bool moving_sphere_bounding_box(const moving_sphere_t* ms, aabb_t *output_box);

#endif /* SPHERE_H */
