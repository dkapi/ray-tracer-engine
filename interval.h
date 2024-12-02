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
interval_t interval_enclose(const interval_t* a, const interval_t* b); 
double interval_size(const interval_t *i);
interval_t interval_offset(const interval_t* ival, double displacement);
int interval_contains(const interval_t *i, double x);
bool interval_surrounds(const interval_t *i, double x);
double interval_clamp(const interval_t *i, double x);
interval_t interval_expand(const interval_t* i, double delta);

#endif /*INTERVAL_H*/