#include "sphere.h"
#include <stdlib.h>

// Sphere hit func
int hit_sphere(const sphere *s, const ray_t *r, double tmin, double tmax, hit_record_t *rec) {
    vec3 oc = vec3_subtract_vec(&r->origin, &s->center);
    double a = vec3_length_squared(&r->dir);
    double half_b = vec3_dot(&oc, &r->dir);
    double c = vec3_length_squared(&oc) - s->radius * s->radius;

    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return 0;
    }

    double sqrt_d = sqrt(discriminant);

    // find the nearest root that lies in range
    double root = (-half_b - sqrt_d) / a;
    if (root < tmin || root > tmax) {
        root = (-half_b + sqrt_d) / a;
        if (root < tmin || root > tmax) {
            return 0;
        }
    }

    rec->t = root;
    rec->p = ray_at(r, rec->t);
    vec3 tmp = vec3_subtract_vec(&rec->p, &s->center);
    vec3 outward_normal = vec3_divide_by_scalar(&tmp, s->radius);
    rec->normal = outward_normal;

    return 1;
}

// Sphere constructor
sphere* sphere_create(const point3 *center, double radius) {
    sphere *s = (sphere *)malloc(sizeof(sphere));
    if (s) {
        s->center = *center;
        s->radius = radius;
        s->base.hit = (hit_fn)hit_sphere;  // assign hit function pointer
    }
    return s;
}