#ifndef OBJECT_INSTANCING_H
#define OBJECT_INSTANCING_H

#include "hittable.h"
#include "vec3.h"

typedef struct translate {
    hittable base;
    hittable* object;
    vec3 offset;
} translate_t;

typedef struct rotate_y {
    hittable base;
    hittable* object;
    double sin_theta;
    double cos_theta;
    bool has_box;
    aabb_t bbox;
} rotate_y_t;


//translating
translate_t* translate_object(hittable* object, const vec3* offset);
bool translate_hit(const translate_t* translate, const ray_t* r, interval_t ray_t, hit_record_t* rec);
bool translate_bounding_box(const translate_t* translate, aabb_t* output_box);

//rotating y axis
rotate_y_t* rotate_object_y(hittable* object, double angle);
bool rotate_y_hit(const rotate_y_t* rotate, const ray_t* r, interval_t ray_t, hit_record_t* rec);
bool rotate_y_bounding_box(const rotate_y_t* rotate, aabb_t* output_box);
#endif /* OBJECT_INSTANCING_H */