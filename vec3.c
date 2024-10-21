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

bool vec3_near_zero(const vec3* u){
    double s = 1e-8;
    return (fabs(u->x) <  s) && (fabs(u->y) <  s) && (fabs(u->z) <  s);
}


inline vec3 vec3_random() {
    return vec3_create_values(random_double(), random_double(), random_double());
}

inline vec3 vec3_random_values(double min, double  max) {
    return vec3_create_values(random_double_with_params(min, max), random_double_with_params(min, max), random_double_with_params(min, max));
}

vec3 vec3_unit_vector(const vec3 *v) {
    return vec3_divide_by_scalar(v, vec3_length(v));
}

vec3 vec3_random_unit_vector() {
    while(1) {
        vec3 p = vec3_random_values(-1,1);
        double lensq = vec3_length_squared(&p);
        if(1e-160 < lensq && lensq <= 1)  {
            return vec3_divide_by_scalar(&p, sqrt(lensq));
        }
    }
}

inline vec3 vec3_random_on_hemisphere(const vec3* normal) {
    vec3 on_unit_sphere = vec3_random_unit_vector();
    if(vec3_dot(&on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    } else {
        return vec3_negate(&on_unit_sphere);
    }
}

vec3 vec3_reflect(const vec3* u, const vec3* v) {
    double dot = vec3_dot(u, v);
    vec3 temp = vec3_multiply_by_scalar(v, 2 * dot);
    return vec3_subtract(u, &temp);
}

vec3 vec3_refract(const vec3 *uv, const vec3 *n, double etai_over_etat) {
    vec3 neg_uv = vec3_negate(uv);
    double cos_theta = fmin(vec3_dot(&neg_uv, n), 1.0);
    vec3 n_scaled = vec3_multiply_by_scalar(n, cos_theta);
    vec3 temp = vec3_add(uv, &n_scaled);
    vec3 r_out_perp = vec3_multiply_by_scalar(&temp, etai_over_etat);
    double length_squared = vec3_length_squared(&r_out_perp);
    vec3 r_out_parallel = vec3_multiply_by_scalar(n, -sqrt(fabs(1.0 - length_squared)));
    return vec3_add(&r_out_perp, &r_out_parallel);
}

inline vec3 random_in_unit_disk() {
    while (true) {
        vec3 p = vec3_create_values(random_double(-1,1), random_double(-1,1), 0);
        if ( vec3_length(&p)< 1)
            return p;
    }
}

