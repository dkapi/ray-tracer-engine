#include "material.h"

// Default scatter implementation (no scattering occurs)
static bool default_scatter(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, scatter_record_t* srec) {
    if (srec) {
        srec->pdf_ptr = NULL;
        srec->skip_pdf = false;
    }
    return false;
}

// Default emitted light (no emission)
static color default_emitted(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, double u, double v, const point3* p) {
    return vec3_create_values(0, 0, 0);
}

// Default scattering PDF (no contribution)
static double default_scattering_pdf(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, const ray_t* scattered) {
    return 0.0;
}

// Lambertian scattering PDF
static double lambertian_scattering_pdf(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, const ray_t* scattered) {
    vec3 unit_dir = vec3_unit_vector(&scattered->dir);
    double cosine = vec3_dot(&rec->normal, &unit_dir);
    return (cosine < 0) ? 0.0 : cosine / PI;
}



// Lambertian material scattering
bool lambertian_scatter(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, scatter_record_t* srec) {
    const lambertian_t* lambertian = (const lambertian_t*)mat;

    srec->attenuation = lambertian->tex->value(lambertian->tex, rec->u, rec->v, &rec->p);
    srec->pdf_ptr = (pdf_t*)cosine_pdf_create(&rec->normal);
    srec->skip_pdf = false;

    return true;
}


static double isotropic_scattering_pdf(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, const ray_t* scattered) {
    return 1.0 / (4.0 * PI);  // Uniform distribution over the sphere
}


// Metal material scattering
bool metal_scatter(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, scatter_record_t* srec) {
    const metal_t* metal = (const metal_t*)mat;

    vec3 unit_dir = vec3_unit_vector(&r_in->dir);
    vec3 reflected = vec3_reflect(&unit_dir, &rec->normal);

    if (metal->fuzz > 0) {
        vec3 rnd_unit = random_unit_vector();
        vec3 fuzz_vec = vec3_multiply_by_scalar(&rnd_unit, metal->fuzz);
        reflected = vec3_add(&reflected, &fuzz_vec);
    }

    if (vec3_dot(&reflected, &rec->normal) <= 0) {
        return false; // No scattering if the reflection is inward
    }

    srec->attenuation = metal->albedo;
    srec->skip_pdf = true;
    srec->skip_pdf_ray = ray_create(&rec->p, &reflected, r_in->time);

    return true;
}

// Reflectance calculation using Schlick's approximation
static double reflectance(double cosine, double refraction_index) {
    double r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

// Dielectric material scattering
bool dielectric_scatter(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, scatter_record_t* srec) {
    const dielectric_t* dielectric = (const dielectric_t*)mat;

    srec->attenuation = vec3_create_values(1.0, 1.0, 1.0);
    srec->pdf_ptr = NULL;
    srec->skip_pdf = true;

    double refraction_ratio = rec->front_face ? (1.0 / dielectric->refraction_index) : dielectric->refraction_index;
    vec3 unit_dir = vec3_unit_vector(&r_in->dir);
    vec3 neg_dir = vec3_negate(&unit_dir);
    double cos_theta = fmin(vec3_dot(&neg_dir, &rec->normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
        direction = vec3_reflect(&unit_dir, &rec->normal);
    } else {
        direction = vec3_refract(&unit_dir, &rec->normal, refraction_ratio);
    }

    srec->skip_pdf_ray = ray_create(&rec->p, &direction, r_in->time);
    return true;
}


// Diffuse light emitted function
static color diffuse_light_emitted(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, double u, double v, const point3* p) {
    const diffuse_light_t* light = (const diffuse_light_t*)mat;
    if (!rec->front_face) {
        return vec3_create_values(0, 0, 0);
    }
    return light->emit->value(light->emit, u, v, p);
}

// Isotropic scattering
bool isotropic_scatter(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, scatter_record_t* srec) {
    const isotropic_t* isotropic = (const isotropic_t*)mat;

    vec3 scatter_dir = random_unit_vector();
    srec->attenuation = isotropic->albedo->value(isotropic->albedo, rec->u, rec->v, &rec->p);
    srec->pdf_ptr = NULL;
    srec->skip_pdf = true;
    srec->skip_pdf_ray = ray_create(&rec->p, &scatter_dir, r_in->time);

    return true;
}

// Factory functions for creating materials
lambertian_t* create_lambertian_texture(texture_t* tex) {
    lambertian_t* lambertian = (lambertian_t*)malloc(sizeof(lambertian_t));
    lambertian->base.scatter = lambertian_scatter;
    lambertian->base.emitted = default_emitted;
    lambertian->base.scattering_pdf = lambertian_scattering_pdf;
    lambertian->tex = tex;
    return lambertian;
}

lambertian_t* create_lambertian_color(const color* albedo) {
    solid_color_t* solid = create_solid_color(albedo->x, albedo->y, albedo->z);
    return create_lambertian_texture((texture_t*)solid);
}

metal_t* create_metal(const color* albedo, double fuzz) {
    metal_t* metal = (metal_t*)malloc(sizeof(metal_t));
    metal->base.scatter = metal_scatter;
    metal->base.emitted = default_emitted;
    metal->base.scattering_pdf = default_scattering_pdf;
    metal->albedo = *albedo;
    metal->fuzz = fmin(fuzz, 1.0);
    return metal;
}

dielectric_t* create_dielectric(double refraction_index) {
    dielectric_t* dielectric = (dielectric_t*)malloc(sizeof(dielectric_t));
    dielectric->base.scatter = dielectric_scatter;
    dielectric->base.emitted = default_emitted;
    dielectric->base.scattering_pdf = default_scattering_pdf;
    dielectric->refraction_index = refraction_index;
    return dielectric;
}

diffuse_light_t* create_diffuse_light_texture(texture_t* tex) {
    diffuse_light_t* light = (diffuse_light_t*)malloc(sizeof(diffuse_light_t));
    light->base.scatter = default_scatter;
    light->base.emitted = diffuse_light_emitted;
    light->base.scattering_pdf = default_scattering_pdf;
    light->emit = tex;
    return light;
}

diffuse_light_t* create_diffuse_light_color(const color* emit_color) {
    texture_t* tex = (texture_t*)create_solid_color(emit_color->x, emit_color->y, emit_color->z);
    return create_diffuse_light_texture(tex);
}

isotropic_t* create_isotropic(texture_t* tex) {
    isotropic_t* isotropic = (isotropic_t*)malloc(sizeof(isotropic_t));
    isotropic->base.scatter = isotropic_scatter;
    isotropic->base.emitted = default_emitted;
    isotropic->base.scattering_pdf = isotropic_scattering_pdf;
    isotropic->albedo = tex;
    return isotropic;
}

// Destroy material
void destroy_material(material_t* mat) {
    free(mat);
}
