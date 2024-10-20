#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec3.h"
#include "hittable.h"


typedef struct triangle {
    hittable base; // Ensure this is first
    point3 a, b, c;
} triangle_t;


triangle_t* create_triangle(const point3* a, const point3* b, const point3* c);
bool hit_triangle(const triangle_t* tri, const ray_t* r, interval_t ray, hit_record_t* rec);


#endif /* TRIANGLE_H */