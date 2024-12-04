#ifndef VOLUMES_H
#define VOLUMES_H

#include "hittable.h"

typedef struct {
    hittable base;
    hittable* boundary;
    double negative_inverse_density;
    material_t* mat;
    aabb_t bbox;
}volume_t;


volume_t* volume_create(hittable* boundary, double density, texture_t* tex);
bool volume_hit(const volume_t* medium, const ray_t* r, interval_t ray, hit_record_t* rec);
bool volume_bounding_box(const volume_t* medium, aabb_t* output_box);

#endif /* VOLUMES_H */