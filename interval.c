#include "interval.h"

// constructor
interval_t interval_create(double min, double max) {
    interval_t i;
    i.min = min;
    i.max = max;
    return i;
}

interval_t interval_empty() {
    return interval_create(+INFINITY, -INFINITY);  
}


interval_t interval_universe() {
    return interval_create(-INFINITY, +INFINITY); 
}


double interval_size(const interval_t *i) {
    return i->max - i->min;
}


int interval_contains(const interval_t *i, double x) {
    return i->min <= x && x <= i->max;
}


int interval_surrounds(const interval_t *i, double x) {
    return i->min < x && x < i->max;
}
