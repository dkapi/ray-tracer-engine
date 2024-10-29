#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.h"

typedef struct aabb {
    interval_t x, y, z;
}aabb_t;


aabb_t aabb_create_empty();
aabb_t aabb_create_with_intervals(const interval_t* x, const interval_t* y, const interval_t* z);
aabb_t aabb_create_with_points(const point3* a, const point3* b);
aabb_t aabb_create_with_boxes(const aabb_t *box0, const aabb_t *box1);
aabb_t aabb_surrounding_box(const aabb_t *box0, const aabb_t *box1);
const interval_t* aabb_axis_interval(const aabb_t* box, int n);
bool aabb_hit(const aabb_t* box, const ray_t* r, interval_t ray_interval);

#endif /* AABB_H */