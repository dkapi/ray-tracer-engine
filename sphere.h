#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

// Sphere structure that includes hittable as a base class
typedef struct {
    hittable base;     // make sure to always have base first
    point3 center;
    double radius;
    material_t* mat;
    aabb_t bbox;
} sphere;


sphere* sphere_create(const point3 *center, double radius, material_t* mat);
bool hit_sphere(const sphere *s, const ray_t *r, interval_t ray, hit_record_t *rec);
bool sphere_bounding_box(const sphere* s, aabb_t *output_box);

#endif /* SPHERE_H */
