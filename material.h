#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdbool.h>
#include <stdlib.h>
#include "ray.h"
#include "color.h"
#include "vec3.h"
#include "hittable.h"

// Forward declaration of `hit_record_t` to ensure it's recognized
typedef struct hit_record hit_record_t;

typedef struct material material_t;

// Definition for scatter function
typedef bool (*scatter_fn)(const material_t* mat, 
                const ray_t* r_in, const hit_record_t* rec, 
                color* attenuation, ray_t* scattered);

struct material {
    scatter_fn scatter; 
};

typedef struct lambertian {
    material_t base;
    color albedo;
} lambertian_t;

typedef struct metal {
    material_t base;
    color albedo;
    double fuzz;
} metal_t;

typedef struct dieletric {
    material_t base;
    double refraction_index;
} dielectric_t;

lambertian_t* create_lambertian(const color* albedo);
metal_t* create_metal(const color* albedo, double fuzz);\
dielectric_t* create_dielectric(double refraction_index);
void destroy_material(material_t* mat);

#endif /* MATERIAL_H */
