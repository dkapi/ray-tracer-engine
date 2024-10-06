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

// sphere hit func
int hit_sphere(const sphere *s, const ray_t *r, double tmin, double tmax, hit_record_t *rec);
// sphere constructor
sphere* sphere_create(const point3 *center, double radius);

#endif /* SPHERE_H */
