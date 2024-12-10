#ifndef PDF_H
#define PDF_H

#include "../vec3.h"
#include "../hdr.h"
#include "onb.h"
#include <math.h>

struct hittable;
typedef struct pdf pdf_t;

// Base PDF structure
typedef struct pdf {
    double (*value)(const struct pdf* pdf, const vec3* direction);
    vec3 (*generate)(const struct pdf* pdf);
} pdf_t;

// Cosine-weighted PDF
typedef struct {
    pdf_t base;
    vec3 normal;
} cosine_pdf_t;

// Mixture PDF
typedef struct mixture_pdf {
    pdf_t base;
    pdf_t* p0;
    pdf_t* p1;
} mixture_pdf_t;

// Hittable-based PDF
typedef struct {
    pdf_t base;
    const struct hittable* object;
    point3 origin;
} hittable_pdf_t;


// Function prototypes for cosine PDF
cosine_pdf_t* cosine_pdf_create(const vec3 *normal);
double cosine_pdf_value(const pdf_t* pdf, const vec3* direction);
vec3 cosine_pdf_generate(const pdf_t* pdf);

// Function prototypes for mixture PDF
mixture_pdf_t* mixture_pdf_create(pdf_t* p0, pdf_t* p1);
double mixture_pdf_value(const pdf_t* pdf, const vec3* direction);
vec3 mixture_pdf_generate(const pdf_t* pdf);
void mixture_pdf_free(mixture_pdf_t* mix);
pdf_t* hittable_pdf_create(const struct hittable* object, const point3* origin);
void hittable_pdf_free(hittable_pdf_t* pdf);

// Utility functions for random direction generation
vec3 random_unit_vector();
vec3 random_cosine_direction();

// Function prototypes for environment sampling
vec3 sample_environment(const vec3 *direction, const hdr_texture_t *hdr_tex);

#endif // IMPORTANCE_SAMPLING_H
