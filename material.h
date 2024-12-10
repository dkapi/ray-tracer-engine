#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdbool.h>
#include <stdlib.h>
#include "ray.h"
#include "color.h"
#include "vec3.h"
#include "hittable.h"
#include "textures/texture.h"
#include "importance_sampling/pdf.h"

// Forward declaration of `hit_record_t` to ensure it's recognized
typedef struct hit_record hit_record_t;
typedef struct scatter_record scatter_record_t;
typedef struct material material_t;

// Definition for scatter function
typedef bool (*scatter_fn)(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, scatter_record_t* srec);
typedef color (*emitted_fn)(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, double u, double v, const point3* p);
typedef double (*scattering_pdf_fn)(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, const ray_t* scattered);


struct material {
    scatter_fn scatter; 
    emitted_fn emitted;
    scattering_pdf_fn scattering_pdf;
};

typedef struct lambertian {
    material_t base;
    texture_t* tex;
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

typedef struct diffuse_light {
    material_t base;
    texture_t* emit;
} diffuse_light_t;

typedef struct isotropic {
    material_t base;
    texture_t* albedo;
}isotropic_t;

typedef struct scatter_record {
    color attenuation;
    pdf_t* pdf_ptr;
    bool skip_pdf;   
    ray_t skip_pdf_ray;
} scatter_record_t;

lambertian_t* create_lambertian_texture(texture_t* tex);
lambertian_t* create_lambertian_color(const color* albedo);
metal_t* create_metal(const color* albedo, double fuzz);
dielectric_t* create_dielectric(double refraction_index);
diffuse_light_t* create_diffuse_light_texture(texture_t* emit_texture);
diffuse_light_t* create_diffuse_light_color(const color* emit_color);
isotropic_t* create_isotropic(texture_t* tex);
void destroy_material(material_t* mat);

#endif /* MATERIAL_H */
