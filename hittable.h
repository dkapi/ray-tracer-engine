#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "vec3.h"
#include "interval.h"
#include "material.h"
#include "darray/d_array.h"
#include "aabb.h"


// Forward declaration of material_t
typedef struct material material_t;

// Define hit_record_t
typedef struct hit_record {
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    material_t* mat;  // Add pointer to material

} hit_record_t;

// function pointer type for hit methods in hittable objects
//TODO: remove this eventually probably, since we dont need it when we use bounding hit
typedef bool (*hit_fn)(const void *self, const ray_t *r, interval_t ray, hit_record_t *rec);

// function pointer type for calculating bounding boxes in hittable objects
typedef bool (*bounding_box_fn)(const void *self, aabb_t *output_box);

// hittable structure definition
typedef struct {
    hit_fn hit;  // function pointer for hit function
    bounding_box_fn bbox; // function pointer for bounding box function
} hittable;

// hittable list structure (dynamic array of hittable objects)
typedef struct {
    hittable base;
    darray *objects;
    aabb_t bbox;
} hittable_list;

hittable_list* hittable_list_create();
void hittable_list_add(hittable_list *list, hittable *object);
void hittable_list_destroy(hittable_list *list);
bool hittable_list_hit(const hittable_list *list, const ray_t *r, interval_t ray, hit_record_t *rec);
bool hittable_list_bounding_box(const hittable_list *list, aabb_t *output_box);
void set_face_normal(hit_record_t *rec, const ray_t *r, const vec3 *outward_normal);

#endif /* HITTABLE_H */
