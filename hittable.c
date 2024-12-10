#include "hittable.h"
#include <stdlib.h>

hittable_list* hittable_list_create() {
    hittable_list *list = (hittable_list *)malloc(sizeof(hittable_list));
    if (list) {
        list->objects = darray_create();
        list->base.hit = (hit_fn)hittable_list_hit;
        list->base.bbox = (bounding_box_fn)hittable_list_bounding_box;
        list->base.pdf_value = (pdf_value_fn)default_pdf_value;
        list->base.random = (random_fn)default_random; 
        list->bbox = aabb_create_empty();
    }
    return list;
}

void hittable_list_add(hittable_list *list, hittable *object) {
    if (list && object) {
        darray_add(list->objects, object);

        aabb_t temp_box;
        if(object->bbox(object, &temp_box)) {
            list->bbox = aabb_create_with_boxes(&list->bbox, &temp_box);
        }
    }
}

void hittable_list_merge(hittable_list *target, const hittable_list *source) {
    if (target && source) {
        for (size_t i = 0; i < source->objects->size; i++) {
            hittable* object = (hittable*)source->objects->data[i];
            hittable_list_add(target, object);
        }
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

aabb_t aabb_surrounding_box(const aabb_t *box0, const aabb_t *box1) {
    point3 small = vec3_create_values(
        fmin(box0->x.min, box1->x.min),
        fmin(box0->y.min, box1->y.min),
        fmin(box0->z.min, box1->z.min)
    );

    point3 big = vec3_create_values(
        fmax(box0->x.max, box1->x.max),
        fmax(box0->y.max, box1->y.max),
        fmax(box0->z.max, box1->z.max)
    );

    return aabb_create_with_points(&small, &big);
}

bool hittable_list_bounding_box(const hittable_list *list, aabb_t *output_box) {
    if (darray_size(list->objects) == 0) {
        return false;
    }

    aabb_t temp_box;
    bool first_box = true;

    // Iterate over all objects to calc bounding box
    for (int i = 0; i < darray_size(list->objects); i++) {
        hittable *object = (hittable *)darray_get(list->objects, i);
        if (object->bbox && object->bbox(object, &temp_box)) {
            *output_box = first_box ? temp_box : aabb_surrounding_box(output_box, &temp_box);
            first_box = false;
        } else {
            return false; 
        }
    }

    return true;
}

void set_face_normal(hit_record_t *rec, const ray_t *r, const vec3 *outward_normal) {
    rec->front_face = vec3_dot(&r->dir, outward_normal) < 0;
    
    if (rec->front_face) {
        rec->normal = *outward_normal;
    } else {
        rec->normal = vec3_multiply_by_scalar(outward_normal, -1);
    }
}



double default_pdf_value(const void *self, const point3 *origin, const vec3 *direction) {
    return 0.0;
}

vec3 default_random(const void *self, const point3 *origin) {
    // Return a default direction (can be improved for specific use cases)
    return vec3_create_values(1, 0, 0);
}

double hittable_list_pdf_value(const hittable_list* list, const point3* origin, const vec3* direction) {
    double weight = 1.0 / darray_size(list->objects);
    double sum = 0.0;

    for (int i = 0; i < darray_size(list->objects); i++) {
        hittable* object = (hittable*)darray_get(list->objects, i);
        if (object && object->pdf_value) {
            sum += weight * object->pdf_value(object, origin, direction);
        }
    }

    return sum;
}

vec3 hittable_list_random(const hittable_list* list, const point3* origin) {
    int index = (int)(random_double() * darray_size(list->objects));
    hittable* object = (hittable*)darray_get(list->objects, index);

    if (object && object->random) {
        return object->random(object, origin);
    }

    return vec3_create_values(1, 0, 0); // Default direction
}
