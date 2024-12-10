#ifndef ONB_H
#define ONB_H

#include "../vec3.h"

typedef struct onb {
    vec3 u; // tangent
    vec3 v; // bitangent
    vec3 w; // normal
} onb_t;

// Functions
void onb_build_from_w(onb_t *onb, const vec3 *w);
vec3 onb_transform(const onb_t* onb, const vec3* a);
vec3 onb_local(const onb_t *onb, const vec3 *a);

#endif // ONB_H