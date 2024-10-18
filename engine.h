#ifndef ENGINE_H
#define ENGINE_H

#include <math.h>
#include <limits.h>
#include <memory.h>
#include <stdio.h>

#include "color.h"
#include "hittable.h"
#include "sphere.h"
#include "camera.h"

#define PI (const double) 3.1415926535897932385


static inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

static inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

static inline double random_double_with_params(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

#endif /*ENGINE_H*/