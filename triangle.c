#include <math.h>
#include <stdlib.h>
#include "triangle.h"



triangle_t* create_triangle(const point3* a, const point3* b, const point3* c, material_t* mat) {
    triangle_t* triangle = (triangle_t*)malloc(sizeof(triangle_t));
    
    triangle->a = *a;
    triangle->b = *b;
    triangle->c = *c;
    triangle->mat = mat;
    triangle->base.hit = (hit_fn)hit_triangle;  //hit function pointer
    triangle->base.bbox = (bounding_box_fn)triangle_bounding_box; //bounding box function

    triangle->bbox.x = interval_create(fmin(a->x, fmin(b->x, c->x)), fmax(a->x, fmax(b->x, c->x)));
    triangle->bbox.y = interval_create(fmin(a->y, fmin(b->y, c->y)), fmax(a->y, fmax(b->y, c->y)));
    triangle->bbox.z = interval_create(fmin(a->z, fmin(b->z, c->z)), fmax(a->z, fmax(b->z, c->z)));

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
    triangle->base.hit = (hit_fn)hit_triangle;  // hit function pointer
    triangle->base.bbox = (bounding_box_fn)triangle_bounding_box; //bounding box function

    triangle->bbox.x = interval_create(fmin(a->x, fmin(b->x, c->x)), fmax(a->x, fmax(b->x, c->x)));
    triangle->bbox.y = interval_create(fmin(a->y, fmin(b->y, c->y)), fmax(a->y, fmax(b->y, c->y)));
    triangle->bbox.z = interval_create(fmin(a->z, fmin(b->z, c->z)), fmax(a->z, fmax(b->z, c->z)));


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


    // compute  texture coordinates (u, v)
    double bary0 = 1.0 - bary1 - bary2;
    rec->u = bary0 * tri->uv_a.x + bary1 * tri->uv_b.x + bary2 * tri->uv_c.x;
    rec->v = bary0 * tri->uv_a.y + bary1 * tri->uv_b.y + bary2 * tri->uv_c.y;

    return true;
}

bool triangle_bounding_box(const triangle_t* tri, aabb_t* output_box) {
    *output_box = tri->bbox; // return the precomputed bounding box
    return true;
}