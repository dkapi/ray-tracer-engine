#ifndef INTERVAL_H
#define INTERVAL_H

#include <math.h>
#include <stdbool.h>

typedef struct {
    double min;
    double max;
} interval_t;

interval_t interval_create(double min, double max);
interval_t interval_empty();
interval_t interval_universe();
double interval_size(const interval_t *i);
int interval_contains(const interval_t *i, double x);
bool interval_surrounds(const interval_t *i, double x);
double interval_clamp(const interval_t *i, double x);

#endif /*INTERVAL_H*/