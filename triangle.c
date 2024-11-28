#include <math.h>
#include <stdlib.h>
#include "triangle.h"



triangle_t* create_triangle(const point3* a, const point3* b, const point3* c, material_t* mat) {
    triangle_t* triangle = (triangle_t*)malloc(sizeof(triangle_t));
    
    triangle->a = *a;
    triangle->b = *b;
    triangle->c = *c;
    triangle->mat = mat;
    triangle->base.hit = (hit_fn)hit_triangle;  // assign hit function pointer

    return triangle;
}

triangle_t* create_triangle_uv(const point3* a, const point3* b, const point3* c, const vec3* uv_a, const vec3* uv_b, const vec3* uv_c, material_t* mat)  {
    triangle_t* triangle = (triangle_t*)malloc(sizeof(triangle_t));

    triangle->a = *a;
    triangle->b = *b;
    triangle->c = *c;
    triangle->uv_a = *uv_a;
    triangle->uv_b = *uv_b;
    triangle->uv_c = *uv_c;
    triangle->mat = mat;
    triangle->base.hit = (hit_fn)hit_triangle;  // assign the hit function pointer
    triangle->base.bbox = (bounding_box_fn)triangle_bounding_box; // assign bounding box function

    double min_x = fmin(triangle->a.x, fmin(triangle->b.x, triangle->c.x));
    double min_y = fmin(triangle->a.y, fmin(triangle->b.y, triangle->c.y));
    double min_z = fmin(triangle->a.z, fmin(triangle->b.z, triangle->c.z));
    double max_x = fmax(triangle->a.x, fmax(triangle->b.x, triangle->c.x));
    double max_y = fmax(triangle->a.y, fmax(triangle->b.y, triangle->c.y));
    double max_z = fmax(triangle->a.z, fmax(triangle->b.z, triangle->c.z));

    // Add padding to prevent zero thickness
    double padding = 1e-5; // Small epsilon
    point3 min = vec3_create_values(min_x - padding, min_y - padding, min_z - padding);
    point3 max = vec3_create_values(max_x + padding, max_y + padding, max_z + padding);



    triangle->bbox = aabb_create_with_points(&min, &max);


    return triangle;
}

// triangle hit function based on Möller–Trumbore_intersection
bool hit_triangle(const triangle_t* tri, const ray_t* r, interval_t ray, hit_record_t* rec) {
    // compute edges of the triangle
    vec3 edge1 = vec3_subtract(&tri->b, &tri->a);
    vec3 edge2 = vec3_subtract(&tri->c, &tri->a);

    // calculate determinant
    vec3 h = vec3_cross(&r->dir, &edge2);
    double determinant = vec3_dot(&edge1, &h);

    // if determinant is near zero, ray is parallel to the triangle plane
    if (fabs(determinant) < 1e-8) {
        return false;
    }

    double f = 1.0 / determinant;
    vec3 s = vec3_subtract(&r->origin, &tri->a);
    double bary1 = f * vec3_dot(&s, &h);

    // check if the intersection lies outside the triangle
    if (bary1 < 0.0 || bary1 > 1.0) {
        return false;
    }

    vec3 q = vec3_cross(&s, &edge1);
    double bary2 = f * vec3_dot(&r->dir, &q);

    // check if the intersection lies outside the triangle
    if (bary2 < 0.0 || (bary1 + bary2) > 1.0) {
        return false;
    }

    // calculate the distance from the ray origin to the intersection point
    double bary3 = f * vec3_dot(&edge2, &q);

    // check if the intersection lies within the valid range of the ray
    if (!(interval_surrounds(&ray, bary3))) {
        return false;
    }

    // update the hit record
    rec->t = bary3;
    rec->p = ray_at(r, rec->t);

    // calculate the outward normal of the triangle
    vec3 outward_normal = vec3_cross(&edge1, &edge2);
    outward_normal = vec3_unit_vector(&outward_normal);
    set_face_normal(rec, r, &outward_normal);
    rec->mat = tri->mat;


    // compute interpolated texture coordinates (u, v)
    double bary0 = 1.0 - bary1 - bary2; // the third barycentric coordinate
    rec->u = bary0 * tri->uv_a.x + bary1 * tri->uv_b.x + bary2 * tri->uv_c.x;
    rec->v = bary0 * tri->uv_a.y + bary1 * tri->uv_b.y + bary2 * tri->uv_c.y;

    return true;
}

bool triangle_bounding_box(const triangle_t* tri, aabb_t* output_box) {
    *output_box = tri->bbox; // return the precomputed bounding box
    return true;
}