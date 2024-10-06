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

// color ray_color(const ray_t* r) 
// {

//     vec3 unit_dir = vec3_unit_vector(&r->dir);
//     //blennding factor
//     double a = 0.5 * unit_dir.y + 1;
//     color white = {0,0,0};
//     color blue = {0.5, 0.7, 1.0};

//     color ret;
//     ret.x = (1.0 - a) * white.x + a * blue.x;
//     ret.y = (1.0 - a) * white.y + a * blue.y;
//     ret.z = (1.0 - a) * white.z + a * blue.z;
//     return ret;
// }

