#include <math.h>
#include <stdlib.h>
#include "triangle.h"



triangle_t* create_triangle(const point3* a, const point3* b, const point3* c, material_t* mat) {
    triangle_t* triangle = (triangle_t*)malloc(sizeof(triangle_t));
    
    triangle->a = *a;
    triangle->b = *b;
    triangle->c = *c;
    triangle->mat = mat;
    triangle->base.hit = (hit_fn)hit_triangle;  // Assign hit function pointer

    return triangle;
}

// triangle hit function based on Möller–Trumbore_intersection
bool hit_triangle(const triangle_t* tri, const ray_t* r, interval_t ray, hit_record_t* rec) {
    // Compute edges of the triangle
    vec3 edge1 = vec3_subtract(&tri->b, &tri->a);
    vec3 edge2 = vec3_subtract(&tri->c, &tri->a);

    // Calculate determinant
    vec3 h = vec3_cross(&r->dir, &edge2);
    double determinant = vec3_dot(&edge1, &h);

    // If determinant is near zero, ray is parallel to the triangle plane
    if (fabs(determinant) < 1e-8) {
        return false;
    }

    double f = 1.0 / determinant;
    vec3 s = vec3_subtract(&r->origin, &tri->a);
    double bary1 = f * vec3_dot(&s, &h);

    // Check if the intersection lies outside the triangle
    if (bary1 < 0.0 || bary1 > 1.0) {
        return false;
    }

    vec3 q = vec3_cross(&s, &edge1);
    double bary2 = f * vec3_dot(&r->dir, &q);

    // Check if the intersection lies outside the triangle
    if (bary2 < 0.0 || (bary1 + bary2) > 1.0) {
        return false;
    }

    // Calculate the distance from the ray origin to the intersection point
    double bary3 = f * vec3_dot(&edge2, &q);

    // Check if the intersection lies within the valid range of the ray
    if (!(interval_surrounds(&ray, bary3))) {
        return false;
    }

    // Update the hit record
    rec->t = bary3;
    rec->p = ray_at(r, rec->t);

    // Calculate the outward normal of the triangle
    vec3 outward_normal = vec3_cross(&edge1, &edge2);
    outward_normal = vec3_unit_vector(&outward_normal);
    set_face_normal(rec, r, &outward_normal);
    rec->mat = tri->mat;

    return true;
}