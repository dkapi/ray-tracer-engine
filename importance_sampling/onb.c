#include "onb.h"

void onb_build_from_w(onb_t *onb, const vec3 *w) {
    onb->w = vec3_unit_vector(w);

    vec3 a = (fabs(onb->w.x) > 0.9) ? (vec3){0, 1, 0} : (vec3){1, 0, 0};
    vec3 w_cross_a = vec3_cross(&onb->w, &a);
    onb->v = vec3_unit_vector(&w_cross_a);
    onb->u = vec3_cross(&onb->w, &onb->v);
}

vec3 onb_local(const onb_t *onb, const vec3 *a) {
    vec3 u_scaled = vec3_multiply_by_scalar(&onb->u, a->x);
    vec3 v_scaled = vec3_multiply_by_scalar(&onb->v, a->y);
    vec3 w_scaled = vec3_multiply_by_scalar(&onb->w, a->z);

    vec3 uv_sum = vec3_add(&u_scaled, &v_scaled);
    return vec3_add(&uv_sum, &w_scaled);
}

vec3 onb_transform(const onb_t* onb, const vec3* a) {

    vec3 u_component = vec3_multiply_by_scalar(&onb->u, a->x);
    vec3 v_component = vec3_multiply_by_scalar(&onb->v, a->y);
    vec3 w_component = vec3_multiply_by_scalar(&onb->w, a->z);

    vec3 u_add_v = vec3_add(&u_component, &v_component);
    return vec3_add(&u_add_v, &w_component);
}
