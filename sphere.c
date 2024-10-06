#include "sphere.h"
#include <math.h>
#include <stdlib.h>

// Constructor for a sphere
sphere* sphere_create(const point3 *center, double radius) {
    sphere *s = (sphere*)malloc(sizeof(sphere)); //idk if i even need to malloc
    s->center = *center;
    s->radius = radius;
    s->hit = hit_sphere; // setting hit function of sperticular object
    return s;
}

// Sphere-specific hit function
double hit_sphere(const sphere* s, const ray_t* r)
{
    vec3 oc = vec3_subtract_vec(&s->center, &r->origin);
    double a = vec3_length_squared(&r->dir);
    double h = vec3_dot(&r->dir, &oc);
    double c = vec3_length_squared(&oc) - (s->radius*s->radius);
    double discriminant = h*h - a*c;
    
    if(discriminant < 0) {
        return -1.0;
    }  
    else{
        //swap to fast inverse square root
        return (h - sqrt(discriminant)) / a;
    }
}
