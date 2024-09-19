#ifndef VEC3_H
#define VEC3_H

#include <stdio.h>

/*
    derived from ray tracing in one weekend book
    only caviate is the book is written in c++
    and i'd like to try in C
*/


typedef struct {
    double e[3];
} vec3;

// Constructors
vec3 vec3_create();
vec3 vec3_create_values(double e0, double e1, double e2);

// Accessors
double vec3_x(const vec3 *v);
double vec3_y(const vec3 *v);
double vec3_z(const vec3 *v);

// Operator functions
vec3 vec3_negate(const vec3 *v);
double vec3_get(const vec3 *v, int i);
double *vec3_get_ref(vec3 *v, int i);
void vec3_add(vec3 *u, const vec3 *v);
void vec3_multiply_scalar(vec3 *v, double t);
void vec3_divide_scalar(vec3 *v, double t);

// Length calculations
double vec3_length(const vec3 *v);
double vec3_length_squared(const vec3 *v);

// Utility functions
void vec3_print(FILE *out, const vec3 *v);
vec3 vec3_add_vec(const vec3 *u, const vec3 *v);
vec3 vec3_subtract_vec(const vec3 *u, const vec3 *v);
vec3 vec3_multiply_vec(const vec3 *u, const vec3 *v);
vec3 vec3_multiply_by_scalar(double t, const vec3 *v);
vec3 vec3_divide_by_scalar(const vec3 *v, double t);
double vec3_dot(const vec3 *u, const vec3 *v);
vec3 vec3_cross(const vec3 *u, const vec3 *v);
vec3 vec3_unit_vector(const vec3 *v);

#endif
