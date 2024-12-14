#include "aabb.h"

#define AABB_MIN_DELTA 0.0001  // minimum size for each interval

static void aabb_pad_to_minimums(aabb_t *box) {
    if (interval_size(&box->x) < AABB_MIN_DELTA) {
        box->x = interval_expand(&box->x, AABB_MIN_DELTA);
    }
    if (interval_size(&box->y) < AABB_MIN_DELTA) {
        box->y = interval_expand(&box->y, AABB_MIN_DELTA);
    }
    if (interval_size(&box->z) < AABB_MIN_DELTA) {
        box->z = interval_expand(&box->z, AABB_MIN_DELTA);
    }
}

aabb_t aabb_create_empty() {
    aabb_t box;
        box.x = interval_empty();
        box.y = interval_empty();
        box.z = interval_empty();
    return box;
}

aabb_t aabb_create_with_intervals(const interval_t* x, const interval_t* y, const interval_t* z) {
    aabb_t box;
    box.x = *x;
    box.y = *y;
    box.z = *z;

    aabb_pad_to_minimums(&box); 
    return box;
}

aabb_t aabb_create_with_points(const point3* a, const point3* b) {
    aabb_t box;
    box.x = (a->x <= b->x) ? interval_create(a->x, b->x) : interval_create(b->x, a->x);
    box.y = (a->y <= b->y) ? interval_create(a->y, b->y) : interval_create(b->y, a->y);
    box.z = (a->z <= b->z) ? interval_create(a->z, b->z) : interval_create(b->z, a->z);

    aabb_pad_to_minimums(&box); 
    return box;
}

aabb_t aabb_create_with_boxes(const aabb_t *box0, const aabb_t *box1) {
    aabb_t box;
    box.x = interval_enclose(&box0->x, &box1->x);
    box.y = interval_enclose(&box0->y, &box1->y);
    box.z = interval_enclose(&box0->z, &box1->z);

    aabb_pad_to_minimums(&box); 
    return box;
}

aabb_t aabb_offset(const aabb_t* bbox, const vec3* offset) {
    return (aabb_t){
        .x = interval_create(bbox->x.min + offset->x, bbox->x.max + offset->x),
        .y = interval_create(bbox->y.min + offset->y, bbox->y.max + offset->y),
        .z = interval_create(bbox->z.min + offset->z, bbox->z.max + offset->z)
    };
}


const interval_t* aabb_axis_interval(const aabb_t* box, int n) {
    if (n == 1) return &(box->y);
    if (n == 2) return &(box->z);
    return &(box->x);
}

bool aabb_hit(const aabb_t* box, const ray_t* r, interval_t ray) {
    const point3* ray_orig = &(r->origin);
    const vec3* ray_dir = &(r->dir);

    // iterate over  x, y, z axies
    for (int axis = 0; axis < 3; axis++) {
        const interval_t* ax = aabb_axis_interval(box, axis);

        double adinv = 1.0 / ((axis == 0) ? ray_dir->x : (axis == 1) ? ray_dir->y : ray_dir->z);
        double t0 = (ax->min - ((axis == 0) ? ray_orig->x : (axis == 1) ? ray_orig->y : ray_orig->z)) * adinv;
        double t1 = (ax->max - ((axis == 0) ? ray_orig->x : (axis == 1) ? ray_orig->y : ray_orig->z)) * adinv;

        if (adinv < 0.0) { // swap t0 and t1 if the ray direction is negative
            double temp = t0;
            t0 = t1;
            t1 = temp;
        }

        ray.min = fmax(ray.min, t0);
        ray.max = fmin(ray.max, t1);

        // we exit early if the ray misses the box
        if (ray.max <= ray.min) {
            return false;
        }
    }
    return true;
}