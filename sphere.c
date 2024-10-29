#include "sphere.h"
#include <stdlib.h>


sphere* sphere_create(const point3 *center, double radius, material_t* mat) {
    sphere *s = (sphere *)malloc(sizeof(sphere));
    if (s == NULL) {
        printf("Failed to allocate memory for sphere.\n");
        return NULL;
    }

    s->center = *center;
    s->radius = radius;
    s->mat = mat;

    // Correctly calculate the bounding box
    vec3 offset = vec3_create_values(radius, radius, radius);
    point3 min = vec3_subtract(center, &offset);
    point3 max = vec3_add(center, &offset);

    s->bbox = aabb_create_with_points(&min, &max);

    s->base.hit = (hit_fn)hit_sphere; 
    s->base.bbox = (bounding_box_fn)sphere_bounding_box;
    return s;
}


// Sphere hit func
bool hit_sphere(const sphere *s, const ray_t *r, interval_t ray, hit_record_t *rec) {
    vec3 oc = vec3_subtract(&r->origin, &s->center);
    double a = vec3_length_squared(&r->dir);
    double half_b = vec3_dot(&oc, &r->dir);
    double c = vec3_length_squared(&oc) - s->radius * s->radius;

    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return false;
    }

    double sqrt_d = sqrt(discriminant);

    // find the nearest root that lies in range
    double root = (-half_b - sqrt_d) / a;
    if (!(interval_surrounds(&ray,  root))) {
        root = (-half_b + sqrt_d) / a;
        if (!(interval_surrounds(&ray,  root))) {
            return false;
        }
    }

    rec->t = root;
    rec->p = ray_at(r, rec->t);
    vec3 tmp = vec3_subtract(&rec->p, &s->center);
    vec3 outward_normal = vec3_divide_by_scalar(&tmp, s->radius);
    rec->normal = outward_normal;
    set_face_normal(rec, r, &outward_normal);
    rec->mat = s->mat;

    return true;
}


bool sphere_bounding_box(const sphere *s, aabb_t *output_box) {
    *output_box = s->bbox;  
    return true;
}