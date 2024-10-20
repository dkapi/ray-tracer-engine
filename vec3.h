#ifndef _VEC3_H_
#define _VEC3_H_

#include <stdio.h>

/*
    derived from ray tracing in one weekend book
    only caviate is the book is written in c++
    and i'd like to try in C
*/

typedef struct vec3 vec3;
//alias for code clarity eventually...
typedef vec3 point3;

typedef struct vec3{
    double x;
    double y;
    double z;
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
void vec3_multiply_scalar(vec3 *v, double t);
void vec3_divide_scalar(vec3 *v, double t);

// Length calculations
double vec3_length(const vec3 *v);
double vec3_length_squared(const vec3 *v);

// Utility functions
void vec3_print(const vec3 *v);
vec3 vec3_add(const vec3 *u, const vec3 *v);
vec3 vec3_subtract(const vec3 *u, const vec3 *v);
vec3 vec3_multiply(const vec3 *u, const vec3 *v);
vec3 vec3_multiply_by_scalar(const vec3 *v, double t);
vec3 vec3_divide_by_scalar(const vec3 *v, double t);
double vec3_dot(const vec3 *u, const vec3 *v);
vec3 vec3_cross(const vec3 *u, const vec3 *v);
vec3 vec3_unit_vector(const vec3 *v);

#endif /* _VEC3_H_*/
