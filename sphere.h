#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

typedef struct sphere sphere;

typedef struct sphere{ 
    point3 center;
    double radius;
    double (*hit)(const sphere* s, const ray_t* r); // funcion pointer to access a shapes hit function
} sphere;

// Sphere-specific hit function
double hit_sphere(const sphere* s, const ray_t* r);
// Sphere constructor
sphere* sphere_create(const point3 *center, double radius);

#endif /* SPHERE_H */
