#include "aabb.h"

#define AABB_MIN_DELTA 0.0001  // Minimum size for each interval

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

const interval_t* aabb_axis_interval(const aabb_t* box, int n) {
    if (n == 1) return &(box->y);
    if (n == 2) return &(box->z);
    return &(box->x);
}

bool aabb_hit(const aabb_t* box, const ray_t* r, interval_t ray) {
    const point3* ray_orig = &(r->origin);
    const vec3* ray_dir = &(r->dir);

    // iterate over each x , y , z, axis
    for (int axis = 0; axis < 3; axis++) {
        // Get the interval for the current axis
        const interval_t* ax = aabb_axis_interval(box, axis);
        double adinv;
        double t0, t1;

        // calculate intersection times for the current axis
        switch (axis) {
            case 0: // x-axis
                adinv = 1.0 / ray_dir->x;
                t0 = (ax->min - ray_orig->x) * adinv;
                t1 = (ax->max - ray_orig->x) * adinv;
                break;
            case 1: // y-axis
                adinv = 1.0 / ray_dir->y;
                t0 = (ax->min - ray_orig->y) * adinv;
                t1 = (ax->max - ray_orig->y) * adinv;
                break;
            case 2: // z-axis
                adinv = 1.0 / ray_dir->z;
                t0 = (ax->min - ray_orig->z) * adinv;
                t1 = (ax->max - ray_orig->z) * adinv;
                break;
            default:
                return false; // default should never happen
        }

        if (t0 > t1) {
            double temp = t0;
            t0 = t1;
            t1 = temp;
        }

        // update the ray interval with the new t0 and t1 values
        if (t0 > ray.min) ray.min = t0;
        if (t1 < ray.max) ray.max = t1;

        // ray misses box if ray interval is invlid
        if (ray.max <= ray.min) {
            return false;
        }
    }
    return true;
}