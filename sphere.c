#include "sphere.h"
#include <stdlib.h>


sphere_t* sphere_create(const point3 *center, double radius, material_t* mat) {
    sphere_t *s = (sphere_t *)malloc(sizeof(sphere_t));
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
bool hit_sphere(const sphere_t *s, const ray_t *r, interval_t ray, hit_record_t *rec) {
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


bool sphere_bounding_box(const sphere_t *s, aabb_t *output_box) {
    *output_box = s->bbox;  
    return true;
}


moving_sphere_t* moving_sphere_create(const point3* c1, const point3* c2, double radius,
                                      double time_start, double time_end, material_t* mat) {
    moving_sphere_t* ms = (moving_sphere_t*)malloc(sizeof(moving_sphere_t));

    if (!ms) {
        printf("Failed to allocate memory for moving sphere.\n");
        return NULL;
    }

    ms->center1 = *c1;
    ms->center2 = *c2;
    ms->radius = radius;
    ms->time_start = time_start;
    ms->time_end = time_end;
    ms->mat = mat;

    // define the bounding box to include both start and end positions
    vec3 radius_vec = vec3_create_values(radius, radius, radius);
    point3 min1 = vec3_subtract(c1, &radius_vec);
    point3 max1 = vec3_add(c1, &radius_vec);
    point3 min2 = vec3_subtract(c2, &radius_vec);
    point3 max2 = vec3_add(c2, &radius_vec);

    aabb_t box1 = aabb_create_with_points(&min1, &max1);
    aabb_t box2 = aabb_create_with_points(&min2, &max2);

    ms->bbox = aabb_create_with_boxes(&box1, &box2);

    // set function pointers
    ms->base.hit = (hit_fn)hit_moving_sphere;
    ms->base.bbox = (bounding_box_fn)moving_sphere_bounding_box;

    return ms;
}

bool hit_moving_sphere(const moving_sphere_t* ms, const ray_t* r, interval_t ray, hit_record_t *rec){
    double time_factor =(r->time - ms->time_start) / (ms->time_end - ms->time_start);
    vec3 tmp_center = vec3_subtract(&ms->center2, &ms->center1);
    vec3 delta_center = vec3_multiply_by_scalar(&tmp_center, time_factor);
    point3 center = vec3_add(&ms->center1, &delta_center);

    vec3 oc = vec3_subtract(&r->origin, &center);
    double a = vec3_length_squared(&r->dir);
    double half_b = vec3_dot(&oc, &r->dir);
    double c = vec3_length_squared(&oc) - ms->radius * ms->radius;

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

    vec3 tmp = vec3_subtract(&rec->p, &center);
    vec3 outward_normal = vec3_divide_by_scalar(&tmp, ms->radius);
    rec->normal = outward_normal;
    set_face_normal(rec, r, &outward_normal);
    rec->mat = ms->mat;

    return true;
}

bool moving_sphere_bounding_box(const moving_sphere_t *ms, aabb_t *output_box) {
    *output_box = ms->bbox;  
    return true;
}
