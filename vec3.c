#include "vec3.h"
#include <math.h>

// Constructors, generally just use internally
vec3 vec3_create() {
     vec3 v = {
        .x = 0,
        .y = 0,
        .z = 0
    };
    return v;
}

vec3 vec3_create_values(double e0, double e1, double e2) {
    vec3 v = {
        .x = e0,
        .y = e1,
        .z = e2
    };
    return v;
}

// Accessors
double vec3_x(const vec3 *v) {
    return v->x;
}

double vec3_y(const vec3 *v) {
    return v->y;
}

double vec3_z(const vec3 *v) {
    return v->z;
}

// Operator functions
vec3 vec3_negate(const vec3 *v) {
    return vec3_create_values(-v->x, -v->y, -v->z);
}

void vec3_multiply_scalar(vec3 *v, double t) {
    v->x *= t;
    v->y *= t;
    v->z *= t;
}

void vec3_divide_scalar(vec3 *v, double t) {
    vec3_multiply_scalar(v, 1.0 / t);
}

// Length calculations
double vec3_length(const vec3 *v) {
    return sqrt(vec3_length_squared(v));
}

double vec3_length_squared(const vec3 *v) {
    return v->x*v->x + v->y*v->y + v->z*v->z;
}

// Utility functions
void vec3_print(const vec3 *v) {
    printf("%f %f %f\n", v->x, v->y, v->z);
}

vec3 vec3_add(const vec3 *u, const vec3 *v) {
    return vec3_create_values(u->x + v->x, u->y + v->y, u->z + v->z);
}

vec3 vec3_subtract(const vec3 *u, const vec3 *v) {
    return vec3_create_values(u->x - v->x, u->y - v->y, u->z - v->z);
}

vec3 vec3_multiply(const vec3 *u, const vec3 *v) {
    return vec3_create_values(u->x * v->x, u->y * v->y, u->z * v->z);
}

vec3 vec3_multiply_by_scalar(const vec3 *v, double t) {
    return vec3_create_values(t * v->x, t * v->y, t * v->z);
}

vec3 vec3_divide_by_scalar(const vec3 *v, double t) {
    return vec3_multiply_by_scalar(v, 1.0 / t);
}

double vec3_dot(const vec3 *u, const vec3 *v) {
    return u->x * v->x + u->y * v->y + u->z * v->z;
}

vec3 vec3_cross(const vec3 *u, const vec3 *v) {
    return vec3_create_values(
        u->y * v->z - u->z * v->y,
        u->z * v->x - u->x * v->z,
        u->x * v->y - u->y * v->x
    );
}

vec3 vec3_unit_vector(const vec3 *v) {
    return vec3_divide_by_scalar(v, vec3_length(v));
}
