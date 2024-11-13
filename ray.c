#include "ray.h"

ray_t ray_create(const vec3* origin, vec3* dir, double time)
{
    ray_t r;
    r.origin = *origin;
    r.dir = *dir;
    r.time = time;
    return r;
}

vec3 ray_at(const ray_t* r, double t)
{
    vec3 scaled_dir = vec3_multiply_by_scalar(&(r->dir), t);
    return vec3_add(&r->origin, &scaled_dir);
    
}



