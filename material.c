#include "material.h"

// Default scatter implementation
static bool default_scatter(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, color* attenuation, ray_t* scattered) {
    return false; 
}

static color default_emitted(const material_t* mat, double u, double v, const point3* p) {
    return vec3_create_values(0, 0, 0); 
}

bool lambertian_scatter(const material_t* mat, const ray_t* r_in, const  hit_record_t * rec, color* attenuation, ray_t* scattered) {
    const lambertian_t* lambertian = (const lambertian_t*)mat;

    vec3 random_vec = vec3_random_unit_vector();
    vec3 scatter_dir = vec3_add(&rec->normal, &random_vec);

    if(vec3_near_zero(&scatter_dir)) {
        scatter_dir =  rec->normal;
    }

    *scattered = ray_create(&rec->p, &scatter_dir, r_in->time);
    *attenuation = lambertian->tex->value(lambertian->tex, rec->u, rec->v, &rec->p);

    return true;
}

bool metal_scatter(const material_t* mat, const ray_t* r_in, const  hit_record_t * rec, color* attenuation, ray_t* scattered) {
    const metal_t* metal = (const metal_t*) mat;
    
    vec3 unit_dir =  vec3_unit_vector(&r_in->dir);
    vec3 reflected = vec3_reflect(&unit_dir, &rec->normal);

    vec3 random_vec = vec3_random_unit_vector();
    vec3 fuzz_v = vec3_multiply_by_scalar(&random_vec, metal->fuzz);
    vec3 scattered_dir = vec3_add(&reflected, &fuzz_v);

    *scattered = ray_create(&rec->p, &scattered_dir, r_in->time);
    *attenuation = metal->albedo;

    return (vec3_dot(&scattered_dir, &rec->normal) > 0);
}


    static double reflectance(double cosine, double refraction_index) {
        //Schlick's approximation for reflectance.
        double r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }

bool dielectric_scatter(const material_t* mat, const ray_t* r_in, const hit_record_t* rec, color* attenuation, ray_t* scattered) {
    const dielectric_t* die = (const dielectric_t*) mat;

    *attenuation = vec3_create_values(1.0,1.0,1.0);
    double rr = rec->front_face ? (1.0/die->refraction_index)  : die->refraction_index;

    vec3  unit_dir = vec3_unit_vector(&r_in->dir);
    vec3 neg_unit_dir = vec3_negate(&unit_dir);
    double cos_theta = fmin(vec3_dot(&neg_unit_dir, &rec->normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta *  cos_theta);

    bool cannot_refract = rr * sin_theta > 1.0;
    vec3 direction;

    if(cannot_refract || reflectance(cos_theta, rr) > random_double() ) {
        direction = vec3_reflect(&unit_dir, &rec->normal);
    } else {
        direction = vec3_refract(&unit_dir, &rec->normal, rr);
    }

    *scattered = ray_create(&rec->p, &direction, r_in->time);
    return true;
}

lambertian_t* create_lambertian_texture(texture_t* tex) {
    lambertian_t* lamb = (lambertian_t*)malloc(sizeof(lambertian_t));

    lamb->base.scatter = lambertian_scatter;
    lamb->base.emitted = default_emitted;
    lamb->tex = tex;

    return lamb;
}

lambertian_t* create_lambertian_color(const color* albedo) {
    solid_color_t* solid = create_solid_color(albedo->x, albedo->y, albedo->z);
    return create_lambertian_texture((texture_t*)solid);
}

metal_t* create_metal(const color* albedo, double fuzz) {
    metal_t* metal = (metal_t*)malloc(sizeof(metal_t));

    metal->base.scatter = metal_scatter;
    metal->base.emitted = default_emitted;
    metal->albedo = *albedo;
    metal->fuzz = fuzz < 1.0 ? fuzz : 1.0;
    
    return metal;
}

dielectric_t* create_dielectric(double r_idx) {
    dielectric_t* die = (dielectric_t*)malloc(sizeof(dielectric_t));
    die->base.scatter = dielectric_scatter;
    die->base.emitted = default_emitted;
    die->refraction_index = r_idx;
    
    return die;
}

//lights, camera, action...
static color diffuse_light_emitted(const material_t* mat, double u, double v, const point3* p) {
    const diffuse_light_t* light = (const diffuse_light_t*)mat;
    return light->emit->value(light->emit, u, v, p);
}

diffuse_light_t* create_diffuse_light_texture(texture_t* emit_texture) {
    diffuse_light_t* light = (diffuse_light_t*)malloc(sizeof(diffuse_light_t));
    light->base.scatter = default_scatter;
    light->base.emitted = diffuse_light_emitted;
    light->emit = emit_texture;
    return light;
}

diffuse_light_t* create_diffuse_light_color(const color* emit_color) {
    texture_t* tex = (texture_t*)create_solid_color(emit_color->x, emit_color->y, emit_color->z);
    return create_diffuse_light_texture(tex);
}
void destroy_material(material_t* mat){
    free(mat);
}
