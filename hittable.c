#include "hittable.h"
#include <stdlib.h>

hittable_list* hittable_list_create() {
    hittable_list *list = (hittable_list *)malloc(sizeof(hittable_list));
    if (list) {
        list->objects = darray_create();
    }
    return list;
}

void hittable_list_add(hittable_list *list, hittable *object) {
    if (list && list->objects) {
        darray_add(list->objects, object);
    }
}

void hittable_list_destroy(hittable_list *list) {
    if (list) {
        darray_destroy(list->objects);
        free(list);
    }
}

bool hittable_list_hit(const hittable_list *list, const ray_t *r, interval_t ray, hit_record_t *rec) {
    int hit_anything = 0;
    double closest_so_far = ray.max;
    hit_record_t temp_rec;

    if (list && list->objects) {
        int object_count = darray_size(list->objects);


        for (int i = 0; i < object_count; i++) {
            hittable *obj = (hittable *)darray_get(list->objects, i);

            if (obj && obj->hit && obj->hit(obj, r, interval_create(ray.min, closest_so_far), &temp_rec)) {
                hit_anything = 1;
                closest_so_far = temp_rec.t;
                *rec = temp_rec;
            }
        }
    }
    return hit_anything;
}
void set_face_normal(hit_record_t *rec, const ray_t *r, const vec3 *outward_normal) {
    rec->front_face = vec3_dot(&r->dir, outward_normal) < 0;
    
    if (rec->front_face) {
        rec->normal = *outward_normal;
    } else {
        rec->normal = vec3_multiply_by_scalar(outward_normal, -1);
    }
}
