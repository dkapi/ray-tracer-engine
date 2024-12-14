#include "perlin.h"

static inline double hermite_smooth(double t) {
    return t * t * (3 - 2 * t);
}

static vec3 random_unit_vector() {
    double z = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
    double r = sqrt(1.0 - z * z);
    double phi = 2.0 * PI * ((double)rand() / RAND_MAX);
    return vec3_create_values(r * cos(phi), r * sin(phi), z);
}

static int* perlin_generate_perm() {
    int* perm = (int*)malloc(PERLIN_POINT_COUNT * sizeof(int));
    for (int i = 0; i < PERLIN_POINT_COUNT; i++) {
        perm[i] = i;
    }
    for (int i = PERLIN_POINT_COUNT - 1; i > 0; i--) {
        int target = rand() % (i + 1);
        int tmp = perm[i];
        perm[i] = perm[target];
        perm[target] = tmp;
    }
    return perm;
}

static vec3* perlin_generate_vectors() {
    vec3* vectors = (vec3*)malloc(PERLIN_POINT_COUNT * sizeof(vec3));
    for (int i = 0; i < PERLIN_POINT_COUNT; i++) {
        vectors[i] = random_unit_vector();
    }
    return vectors;
}

static double perlin_interp(const vec3 c[2][2][2], double u, double v, double w) {
    double uu = hermite_smooth(u);
    double vv = hermite_smooth(v);
    double ww = hermite_smooth(w);
    double accum = 0.0;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                vec3 weight = vec3_create_values(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu)) *
                         (j * vv + (1 - j) * (1 - vv)) *
                         (k * ww + (1 - k) * (1 - ww)) *
                         vec3_dot(&c[i][j][k], &weight);
            }
        }
    }

    return accum;
}

perlin_t* perlin_create() {
    perlin_t* perlin = (perlin_t*)malloc(sizeof(perlin_t));
    perlin->random_vectors = perlin_generate_vectors();
    perlin->perm_x = perlin_generate_perm();
    perlin->perm_y = perlin_generate_perm();
    perlin->perm_z = perlin_generate_perm();
    return perlin;
}

//trilinear interpolation
double perlin_noise(const perlin_t* perlin, const point3* p) {
    int i = (int)floor(p->x) & 255;
    int j = (int)floor(p->y) & 255;
    int k = (int)floor(p->z) & 255;

    double u = p->x - floor(p->x);
    double v = p->y - floor(p->y);
    double w = p->z - floor(p->z);

    vec3 c[2][2][2];
    for (int di = 0; di < 2; di++) {
        for (int dj = 0; dj < 2; dj++) {
            for (int dk = 0; dk < 2; dk++) {
                c[di][dj][dk] = perlin->random_vectors[
                    perlin->perm_x[(i + di) & 255] ^
                    perlin->perm_y[(j + dj) & 255] ^
                    perlin->perm_z[(k + dk) & 255]
                ];
            }
        }
    }

    return perlin_interp(c, u, v, w);
}

double perlin_turbulence(const perlin_t* perlin, const point3* p, int depth) {
    double accum = 0.0;
    point3 temp_p = *p;
    double weight = 1.0;

    for (int i = 0; i < depth; i++) {
        accum += weight * perlin_noise(perlin, &temp_p);
        weight *= 0.5;
        temp_p = vec3_multiply_by_scalar(&temp_p, 2.0);
    }

    return fabs(accum);

}

void perlin_free(perlin_t* perlin) {
    free(perlin->random_vectors);
    free(perlin->perm_x);
    free(perlin->perm_y);
    free(perlin->perm_z);
    free(perlin);
}

