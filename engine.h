#ifndef ENGINE_H
#define ENGINE_H

#include <math.h>
#include <limits.h>
#include <memory.h>
#include <stdio.h>

#include "color.h"
#include "hittable.h"
#include "sphere.h"

#define PI (const double) 3.1415926535897932385


inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

#endif /*ENGINE_H*/