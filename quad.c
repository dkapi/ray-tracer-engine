#include "quad.h"

quads_t* quad_create(const point3* Q, const vec3* u, const vec3* v, material_t* mat) {
    quads_t* quad = (quads_t*)malloc(sizeof(quads_t));
    quad->Q = *Q;
    quad->u = *u;
    quad->v = *v;
    quad->mat = mat;
    quad->base.bbox = (bounding_box_fn)quad_bounding_box;
    quad->base.hit = (hit_fn)hit_quad;

    // compute normal, D, and w
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


hittable* create_box(const point3* min, const point3* max, material_t* mat) {
    hittable_list* box_faces = hittable_list_create();

    vec3 dx = (vec3){max->x - min->x, 0, 0};
    vec3 dy = (vec3){0, max->y - min->y, 0};
    vec3 dz = (vec3){0, 0, max->z - min->z};

    // Front face
    point3 front_origin = {min->x, min->y, max->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&front_origin, &dx, &dy, mat));
    // Back face
    point3 back_origin = {min->x, min->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&back_origin, &dx, &dy, mat));
    // Left face
    point3 left_origin = {min->x, min->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&left_origin, &dz, &dy, mat));
    // Right face
    point3 right_origin = {max->x, min->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&right_origin, &dz, &dy, mat));
    // Top face
    point3 top_origin = {min->x, max->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&top_origin, &dx, &dz, mat));
    // Bottom face
    point3 bottom_origin = {min->x, min->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&bottom_origin, &dx, &dz, mat));

    return (hittable*)box_faces;
}

hittable* create_textured_box(const point3* min, const point3* max, material_t* top_mat, material_t* side_mat, material_t* bottom_mat) {
    hittable_list* box_faces = hittable_list_create();

    vec3 dx = (vec3){max->x - min->x, 0, 0};
    vec3 dy = (vec3){0, max->y - min->y, 0};
    vec3 dz = (vec3){0, 0, max->z - min->z};

    // Top face
    point3 top_origin = {min->x, max->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&top_origin, &dx, &dz, top_mat));
    
    // Bottom face
    point3 bottom_origin = {min->x, min->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&bottom_origin, &dx, &dz, bottom_mat));
    
    // Front face
    point3 front_origin = {min->x, min->y, max->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&front_origin, &dx, &dy, side_mat));
    
    // Back face
    point3 back_origin = {min->x, min->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&back_origin, &dx, &dy, side_mat));
    
    // Left face
    point3 left_origin = {min->x, min->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&left_origin, &dz, &dy, side_mat));
    
    // Right face
    point3 right_origin = {max->x, min->y, min->z};
    hittable_list_add(box_faces, (hittable*)quad_create(&right_origin, &dz, &dy, side_mat));

    return (hittable*)box_faces;
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

    // set hit record
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