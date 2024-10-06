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

bool hittable_list_hit(const hittable_list *list, const ray_t *r, double ray_tmin, double ray_tmax, hit_record_t *rec) {
    int hit_anything = 0;
    double closest_so_far = ray_tmax;
    hit_record_t temp_rec;

    if (list && list->objects) {
        for (int i = 0; i < darray_size(list->objects); i++) {
            hittable *obj = (hittable *)darray_get(list->objects, i);
            if (obj->hit(obj, r, ray_tmin, closest_so_far, &temp_rec)) {
                hit_anything = 1;
                closest_so_far = temp_rec.t;
                *rec = temp_rec;
            }
        }
    }
    return hit_anything;
}
