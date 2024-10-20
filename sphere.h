#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

// Sphere structure that includes hittable as a base class
typedef struct {
    hittable base;     // Inherit from hittable
    point3 center;
    double radius;
} sphere;


// sphere constructor
sphere* sphere_create(const point3 *center, double radius);
// sphere hit func
bool hit_sphere(const sphere *s, const ray_t *r, interval_t ray, hit_record_t *rec);

#endif /* SPHERE_H */
