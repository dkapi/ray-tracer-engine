#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "vec3.h"
#include "interval.h"
#include "material.h"
#include "darray/d_array.h"

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

// Function pointer type for hit methods in hittable objects
typedef bool (*hit_fn)(const void *self, const ray_t *r, interval_t ray, hit_record_t *rec);

// Hittable structure definition
typedef struct {
    hit_fn hit;  // Function pointer for hit function
} hittable;

// Hittable list structure (dynamic array of hittable objects)
typedef struct {
    hittable base;
    darray *objects;
} hittable_list;

hittable_list* hittable_list_create();
void hittable_list_add(hittable_list *list, hittable *object);
void hittable_list_destroy(hittable_list *list);
bool hittable_list_hit(const hittable_list *list, const ray_t *r, interval_t ray, hit_record_t *rec);
void set_face_normal(hit_record_t *rec, const ray_t *r, const vec3 *outward_normal);

#endif /* HITTABLE_H */
