#include "volumes.h"


volume_t* volume_create(hittable* boundary, double density, texture_t* tex) {
    volume_t* volume = (volume_t*)malloc(sizeof(volume_t));

    volume->boundary = boundary;
    volume->negative_inverse_density = - 1.0 / density;
    volume->mat = (material_t*)create_isotropic(tex); 

    volume->base.bbox = (bounding_box_fn)volume_bounding_box;
    volume->base.hit = (hit_fn)volume_hit;

    return volume;
}

bool volume_hit(const volume_t* medium, const ray_t* r, interval_t ray, hit_record_t* rec) {
    hit_record_t rec1, rec2;

    if(!medium->boundary->hit(medium->boundary, r, interval_universe(), &rec1)) {
        return false;
    }

    if(!medium->boundary->hit(medium->boundary, r, interval_create(rec1.t + 0.0001, INFINITY), &rec2)) {
        return false;
    }

    if(rec1.t < ray.min) rec1.t = ray.min;
    if(rec2.t > ray.max) rec2.t = ray.max;

    if(rec1.t >= rec2.t) return false;
    if(rec1.t < 0) rec1.t = 0;

    double ray_length = vec3_length(&r->dir);
    double distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    double hit_distance = medium->negative_inverse_density *  log(random_double());

    if(hit_distance > distance_inside_boundary) return false;

    rec->t = rec1.t + hit_distance / ray_length;
    rec->p =  ray_at(r, rec->t);
    rec->normal = vec3_create_values(1,0,0);
    rec->front_face = true;
    rec->mat = medium->mat;

    return true;
}

bool volume_bounding_box(const volume_t* medium, aabb_t* output_box) {
    return medium->boundary->bbox(medium->boundary, output_box);
}

