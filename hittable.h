#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "vec3.h"
#include "darray/d_array.h"
#include <stdbool.h>
#include "interval.h"

// struct to store hit information
typedef struct hit_record {
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
} hit_record_t;

// function pointer for hit method
typedef int (*hit_fn)(const void *self, const ray_t *r, interval_t ray, hit_record_t *rec);

// hittable base struct to hold the hit function of the perticular shape
typedef struct {
    hit_fn hit;
} hittable;

// hittable dynamic array
typedef struct {
    hittable base;
    darray *objects;  // dynamic array to hold different hittable objects
} hittable_list;

// hittable list functions
hittable_list* hittable_list_create();
void hittable_list_add(hittable_list *list, hittable *object);
void hittable_list_destroy(hittable_list *list);
bool hittable_list_hit(const hittable_list *list, const ray_t *r, interval_t ray, hit_record_t *rec);
void set_face_normal(hit_record_t *rec, const ray_t *r, const vec3 *outward_normal);

#endif /*HITTABLE_H*/
