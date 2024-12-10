#include "pdf.h"
#include "../hittable.h"
#include <math.h>
#include <stdlib.h>

// ================================================================
// Cosine-Weighted Hemisphere Sampling
// ================================================================

double cosine_pdf_value(const pdf_t* pdf, const vec3* direction) {
    const cosine_pdf_t* cosine_pdf = (const cosine_pdf_t*)pdf;
    vec3 temp_w = vec3_unit_vector(direction);
    double cosine = vec3_dot(&temp_w, &cosine_pdf->normal);
    return (cosine > 0) ? cosine / PI : 0.0;
}

vec3 cosine_pdf_generate(const pdf_t* pdf) {
    const cosine_pdf_t* cosine_pdf = (const cosine_pdf_t*)pdf;
    vec3 random_dir = random_cosine_direction();

    onb_t uvw;
    onb_build_from_w(&uvw, &cosine_pdf->normal);
    return onb_transform(&uvw, &random_dir);
}

cosine_pdf_t* cosine_pdf_create(const vec3 *normal) {
    cosine_pdf_t* pdf = (cosine_pdf_t*)malloc(sizeof(cosine_pdf_t));
    if (!pdf) return NULL;

    pdf->base.value = cosine_pdf_value;
    pdf->base.generate = cosine_pdf_generate;
    pdf->normal = *normal;
    return pdf;
}

// ================================================================
// Sphere PDF
// ================================================================

// static double sphere_pdf_value(const pdf_t* pdf, const vec3* direction) {
//     return 1.0 / (4.0 * PI);  // Uniform sampling over the sphere
// }

// static vec3 sphere_pdf_generate(const pdf_t* pdf) {
//     return random_unit_vector();
// }

// sphere_pdf_t* sphere_pdf_create() {
//     sphere_pdf_t* pdf = (sphere_pdf_t*)malloc(sizeof(sphere_pdf_t));
//     if (!pdf) return NULL;

//     pdf->base.value = sphere_pdf_value;
//     pdf->base.generate = sphere_pdf_generate;
//     return pdf;
// }

// void sphere_pdf_free(sphere_pdf_t* pdf) {
//     if (pdf) free(pdf);
// }

// ================================================================
// Mixture PDF
// ================================================================

double mixture_pdf_value(const pdf_t* pdf, const vec3* direction) {
    const mixture_pdf_t* mix = (const mixture_pdf_t*)pdf;
    return 0.5 * mix->p0->value(mix->p0, direction) +
           0.5 * mix->p1->value(mix->p1, direction);
}

vec3 mixture_pdf_generate(const pdf_t* pdf) {
    const mixture_pdf_t* mix = (const mixture_pdf_t*)pdf;
    if (random_double() < 0.5) {
        return mix->p0->generate(mix->p0);
    } else {
        return mix->p1->generate(mix->p1);
    }
}

mixture_pdf_t* mixture_pdf_create(pdf_t* p0, pdf_t* p1) {
    mixture_pdf_t* mix = (mixture_pdf_t*)malloc(sizeof(mixture_pdf_t));
    if (!mix) return NULL;

    mix->p0 = p0;
    mix->p1 = p1;
    mix->base.value = mixture_pdf_value;
    mix->base.generate = mixture_pdf_generate;
    return mix;
}

void mixture_pdf_free(mixture_pdf_t* mix) {
    if (mix) free(mix);
}

// ================================================================
// Hittable PDF
// ================================================================

static double hittable_pdf_value(const pdf_t* pdf, const vec3* direction) {
    const hittable_pdf_t* hittable_pdf = (const hittable_pdf_t*)pdf;
    return hittable_pdf->object->pdf_value(hittable_pdf->object, &hittable_pdf->origin, direction);
}

static vec3 hittable_pdf_generate(const pdf_t* pdf) {
    const hittable_pdf_t* hittable_pdf = (const hittable_pdf_t*)pdf;
    return hittable_pdf->object->random(hittable_pdf->object, &hittable_pdf->origin);
}

pdf_t* hittable_pdf_create(const hittable* object, const point3* origin) {
    hittable_pdf_t* pdf = (hittable_pdf_t*)malloc(sizeof(hittable_pdf_t));
    if (!pdf) return NULL;

    pdf->object = object;
    pdf->origin = *origin;
    pdf->base.value = hittable_pdf_value;
    pdf->base.generate = hittable_pdf_generate;
    return (pdf_t*)pdf;
}

void hittable_pdf_free(hittable_pdf_t* pdf) {
    if (pdf) free(pdf);
}

// ================================================================
// Random Direction Generators
// ================================================================

vec3 random_unit_vector() {
    double theta = 2.0 * PI * random_double();
    double z = 2.0 * random_double() - 1.0;
    double r = sqrt(1.0 - z * z);

    return vec3_create_values(r * cos(theta), r * sin(theta), z);
}

vec3 random_cosine_direction() {
    double r1 = random_double();
    double r2 = random_double();

    double z = sqrt(1 - r2);
    double phi = 2 * PI * r1;
    double x = cos(phi) * sqrt(r2);
    double y = sin(phi) * sqrt(r2);

    return vec3_create_values(x, y, z);
}

// ================================================================
// Environment Sampling
// ================================================================

vec3 sample_environment(const vec3 *direction, const hdr_texture_t *hdr_tex) {
    double u, v;
    direction_to_uv(direction, &u, &v);
    return sample_hdr(hdr_tex, u, v);
}
