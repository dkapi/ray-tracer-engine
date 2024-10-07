#ifndef INTERVAL_H
#define INTERVAL_H

#include <math.h>

typedef struct {
    double min;
    double max;
} interval_t;

// Function prototypes
interval_t interval_create(double min, double max);
interval_t interval_empty();
interval_t interval_universe();
double interval_size(const interval_t *i);
int interval_contains(const interval_t *i, double x);
int interval_surrounds(const interval_t *i, double x);

#endif /*INTERVAL_H*/