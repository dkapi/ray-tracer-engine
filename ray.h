#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include <math.h>

#define RAY_T_MIN 0.0001f // edge case for rays boucing off objects to not intersect on its own object
#define RAY_T_MAX 1.0e30f // just using this as a possible max, or our "infinite"

typedef struct {
    vec3 origin; //start
    vec3 dir;
}ray_t;

ray_t ray_create(vec3* origin, vec3* dir);
vec3 ray_at(ray_t* r, double t);


#endif /* RAY_H */
