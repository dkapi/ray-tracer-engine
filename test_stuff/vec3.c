#include "vec3.h"
#include <math.h>

// Constructors
vec3 vec3_create() {
    vec3 v = {{0, 0, 0}};
    return v;
}

vec3 vec3_create_values(double e0, double e1, double e2) {
    vec3 v = {{e0, e1, e2}};
    return v;
}

// Accessors
double vec3_x(const vec3 *v) {
    return v->e[0];
}

double vec3_y(const vec3 *v) {
    return v->e[1];
}

double vec3_z(const vec3 *v) {
    return v->e[2];
}

// Operator functions
vec3 vec3_negate(const vec3 *v) {
    return vec3_create_values(-v->e[0], -v->e[1], -v->e[2]);
}

double vec3_get(const vec3 *v, int i) {
    return v->e[i];
}

double *vec3_get_ref(vec3 *v, int i) {
    return &(v->e[i]);
}

void vec3_add(vec3 *u, const vec3 *v) {
    u->e[0] += v->e[0];
    u->e[1] += v->e[1];
    u->e[2] += v->e[2];
}

void vec3_multiply_scalar(vec3 *v, double t) {
    v->e[0] *= t;
    v->e[1] *= t;
    v->e[2] *= t;
}

void vec3_divide_scalar(vec3 *v, double t) {
    vec3_multiply_scalar(v, 1.0 / t);
}

// Length calculations
double vec3_length(const vec3 *v) {
    return sqrt(vec3_length_squared(v));
}

double vec3_length_squared(const vec3 *v) {
    return v->e[0]*v->e[0] + v->e[1]*v->e[1] + v->e[2]*v->e[2];
}

// Utility functions
void vec3_print(FILE *out, const vec3 *v) {
    fprintf(out, "%f %f %f\n", v->e[0], v->e[1], v->e[2]);
}

vec3 vec3_add_vec(const vec3 *u, const vec3 *v) {
    return vec3_create_values(u->e[0] + v->e[0], u->e[1] + v->e[1], u->e[2] + v->e[2]);
}

vec3 vec3_subtract_vec(const vec3 *u, const vec3 *v) {
    return vec3_create_values(u->e[0] - v->e[0], u->e[1] - v->e[1], u->e[2] - v->e[2]);
}

vec3 vec3_multiply_vec(const vec3 *u, const vec3 *v) {
    return vec3_create_values(u->e[0] * v->e[0], u->e[1] * v->e[1], u->e[2] * v->e[2]);
}

vec3 vec3_multiply_by_scalar(double t, const vec3 *v) {
    return vec3_create_values(t * v->e[0], t * v->e[1], t * v->e[2]);
}

vec3 vec3_divide_by_scalar(const vec3 *v, double t) {
    return vec3_multiply_by_scalar(1.0 / t, v);
}

double vec3_dot(const vec3 *u, const vec3 *v) {
    return u->e[0] * v->e[0] + u->e[1] * v->e[1] + u->e[2] * v->e[2];
}

vec3 vec3_cross(const vec3 *u, const vec3 *v) {
    return vec3_create_values(
        u->e[1] * v->e[2] - u->e[2] * v->e[1],
        u->e[2] * v->e[0] - u->e[0] * v->e[2],
        u->e[0] * v->e[1] - u->e[1] * v->e[0]
    );
}

vec3 vec3_unit_vector(const vec3 *v) {
    return vec3_divide_by_scalar(v, vec3_length(v));
}
