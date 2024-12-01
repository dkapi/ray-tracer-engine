#ifndef PERLIN_H
#define PERLIN_H

#include "vec3.h"
#include "math.h"
#define PERLIN_POINT_COUNT 256

typedef struct {
    vec3* random_vectors;
    int* perm_x;
    int* perm_y;
    int* perm_z;
} perlin_t;

perlin_t* perlin_create();
double perlin_noise(const perlin_t* perlin, const point3* p);
double perlin_turbulence(const perlin_t* perlin, const point3* p, int depth);
void perlin_free(perlin_t* perlin);

#endif /* PERLIN_H */