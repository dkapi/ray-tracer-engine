#include "ray.h"

ray_t ray_create(vec3* origin, vec3* dir)
{
    ray_t r;
    r.origin = *origin;
    r.dir = *dir;
    return r;
}

vec3 ray_at(const ray_t* r, double t)
{
    vec3 scaled_dir = vec3_multiply_by_scalar(&(r->dir), t);
    return vec3_add_vec(&r->origin, &scaled_dir);
    
}



