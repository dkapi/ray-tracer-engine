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

interval_t interval_enclose(const interval_t* a, const interval_t* b) {
    interval_t i;
    i.min = (a->min <= b->min) ? a->min : b->min;
    i.max = (a->max >= b->max) ? a->max : b->max;
    return i;
}

double interval_size(const interval_t *i) {
    return i->max - i->min;
}

int interval_contains(const interval_t *i, double x) {
    return i->min <= x && x <= i->max;
}

bool interval_surrounds(const interval_t *i, double x) {
    return i->min < x && x < i->max;
}

interval_t interval_offset(const interval_t* ival, double displacement) {
    return interval_create(ival->min + displacement, ival->max + displacement);
}

double interval_clamp(const interval_t *i, double x) {
    if(x < i->min) {
        return i->min;
    } 
    else if(x > i->max) {
        return  i->max;
    }
    return x;
}

interval_t interval_expand(const  interval_t* i, double delta) {
    double padding = delta/2;
    return interval_create(i->min - padding, i->max + padding);
}