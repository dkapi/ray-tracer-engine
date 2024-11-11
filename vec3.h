#ifndef _VEC3_H_
#define _VEC3_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PI (const double) 3.1415926535897932385


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
bool vec3_near_zero(const vec3 *u);

// random values, and vectors, moved from engine.h to here since all files use vec3
static inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

static inline double random_double_in_range(double min, double max) {
    return min + (max-min)*random_double();
}
//  uhh maybe make static inline too
vec3 vec3_random();
vec3 vec3_random_values(double min, double max);

vec3 vec3_unit_vector(const vec3 *v);
vec3 vec3_random_unit_vector();
vec3 vec3_random_on_hemisphere(const vec3* normal);
vec3 vec3_reflect(const vec3* u, const vec3* v);
vec3 vec3_refract(const vec3* u, const vec3* v, double etai_over_etat);
vec3 random_in_unit_disk();


#endif /* _VEC3_H_*/
