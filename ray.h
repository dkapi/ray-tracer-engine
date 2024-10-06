#ifndef RAY_H
#define RAY_H

#include <math.h>
#include "vec3.h"

#define RAY_T_MIN 0.0001f // edge case for rays boucing off objects to not intersect on its own object
#define RAY_T_MAX 1.0e30f // just using this as a possible default, or our "infinite"

//forward declaration
typedef struct ray_t ray_t;

typedef struct ray_t{
    vec3 origin; //start
    vec3 dir;
}ray_t;

ray_t ray_create(vec3* origin, vec3* dir);
//returns the ray at the given point
vec3 ray_at(const ray_t* r, double t);
//color ray_color(const ray_t* r);


#endif /* RAY_H */
