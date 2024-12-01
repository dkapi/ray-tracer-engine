#include "quad.h"

quads_t* quad_create(const point3* Q, const vec3* u, const vec3* v, material_t* mat) {
    quads_t* quad = (quads_t*)malloc(sizeof(quads_t));
    quad->Q = *Q;
    quad->u = *u;
    quad->v = *v;
    quad->mat = mat;
    quad->base.bbox = (bounding_box_fn)quad_bounding_box;
    quad->base.hit = (hit_fn)hit_quad;

    // Compute normal, D, and w
    vec3 tempcross = vec3_cross(u, v);
    quad->normal = vec3_unit_vector(&tempcross);
    quad->D = vec3_dot(&quad->normal, Q);

    vec3 n = vec3_cross(u, v);
    double n_dot_n = vec3_dot(&n, &n);
    quad->w = vec3_divide_by_scalar(&n, n_dot_n);

    // computing bounding box
    point3 corner1 = *Q;                     // bottom-left corner
    point3 corner2 = vec3_add(Q, u);         // bottom-right corner
    point3 corner3 = vec3_add(Q, v);         // top-left corner
    point3 corner4 = vec3_add(&corner2, v);  // top-right corner

    // creating a boounding box using the corners
    aabb_t box1 = aabb_create_with_points(&corner1, &corner2);
    aabb_t box2 = aabb_create_with_points(&corner3, &corner4);
    quad->bbox = aabb_create_with_boxes(&box1, &box2);
    
    return quad;
}

bool quad_bounding_box(const quads_t* quad, aabb_t* output_box) {
    *output_box = quad->bbox;
    return true;
}

bool hit_quad(const quads_t* quad, const ray_t* r, interval_t ray, hit_record_t* rec) {
    double denom = vec3_dot(&quad->normal, &r->dir);
    // check if ray is parallel to plane
    if (fabs(denom) < 1e-8) {
        return false;
    }

    // computing intersection t
    double t = (quad->D - vec3_dot(&quad->normal, &r->origin)) / denom;
    if (!(interval_contains(&ray, t))) {
        return false;
    }

    // checking intersection point=
    point3 intersection = ray_at(r, t);
    vec3 planar_hitpt_vector = vec3_subtract(&intersection, &quad->Q);
    vec3 tempcross = vec3_cross(&planar_hitpt_vector, &quad->v);
    double alpha = vec3_dot(&quad->w, &tempcross);
    tempcross = vec3_cross(&quad->u, &planar_hitpt_vector);
    double beta = vec3_dot(&quad->w, &tempcross);

    if (!quad_is_interior(alpha, beta, rec)) {
        return false;
    }

    // Set hit record
    rec->t = t;
    rec->p = intersection;
    rec->mat = quad->mat;
    set_face_normal(rec, r, &quad->normal);
    rec->u = alpha;
    rec->v = beta;

    return true;
}

bool quad_is_interior(double alpha, double beta, hit_record_t* rec) {
    interval_t unit_interval = interval_create(0, 1);

    if (!interval_contains(&unit_interval, alpha) || !interval_contains(&unit_interval, beta)) {
        return false;
    }

    rec->u = alpha;
    rec->v = beta;
    return true;
}