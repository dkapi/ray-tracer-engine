#include "object_instancing.h"


translate_t* translate_object(hittable* object, const vec3* offset) {
    translate_t* translate = (translate_t*)malloc(sizeof(translate_t));
    translate->object = object;
    translate->offset = *offset;
    translate->base.hit = (hit_fn)translate_hit;
    translate->base.bbox = (bounding_box_fn)translate_bounding_box;
    return translate;
}

bool translate_hit(const translate_t* translate, const ray_t* r, interval_t ray, hit_record_t* rec) {
    vec3 offset_origin = vec3_subtract(&r->origin, &translate->offset);
    ray_t offset_ray = ray_create(&offset_origin, (vec3*)&r->dir, r->time);

    if (!translate->object->hit(translate->object, &offset_ray, ray, rec)) {
        return false;
    }

    rec->p = vec3_add(&rec->p, &translate->offset);
    return true;
}

bool translate_bounding_box(const translate_t* translate, aabb_t* output_box) {
    if (!translate->object->bbox(translate->object, output_box)) {
        return false;
    }

    *output_box = aabb_offset(output_box, &translate->offset);
    return true;
}


//rotating
rotate_y_t* rotate_object_y(hittable* object, double angle) {
    rotate_y_t* rotate = (rotate_y_t*)malloc(sizeof(rotate_y_t));
    double radians = degrees_to_radians(angle);

    rotate->sin_theta = sin(radians);
    rotate->cos_theta = cos(radians);
    rotate->object = object;
    rotate->base.hit = (hit_fn)rotate_y_hit;
    rotate->base.bbox = (bounding_box_fn)rotate_y_bounding_box;

    // cmpute bounding box
    rotate->has_box = object->bbox(object, &rotate->bbox);
    if (rotate->has_box) {
        point3 min = {INFINITY, INFINITY, INFINITY};
        point3 max = {-INFINITY, -INFINITY, -INFINITY};

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    double x = i ? rotate->bbox.x.max : rotate->bbox.x.min;
                    double y = j ? rotate->bbox.y.max : rotate->bbox.y.min;
                    double z = k ? rotate->bbox.z.max : rotate->bbox.z.min;

                    double newx = rotate->cos_theta * x + rotate->sin_theta * z;
                    double newz = -rotate->sin_theta * x + rotate->cos_theta * z;

                    point3 tester = {newx, y, newz};
                    min = vec3_min(&min, &tester);
                    max = vec3_max(&max, &tester);
                }
            }
        }
        rotate->bbox = aabb_create_with_points(&min, &max);
    }

    return rotate;
}

bool rotate_y_hit(const rotate_y_t* rotate, const ray_t* r, interval_t ray, hit_record_t* rec) {

    point3 origin = {
        .x = rotate->cos_theta * r->origin.x - rotate->sin_theta * r->origin.z,
        .y = r->origin.y,
        .z = rotate->sin_theta * r->origin.x + rotate->cos_theta * r->origin.z
    };

    vec3 direction = {
        .x = rotate->cos_theta * r->dir.x - rotate->sin_theta * r->dir.z,
        .y = r->dir.y,
        .z = rotate->sin_theta * r->dir.x + rotate->cos_theta * r->dir.z
    };

    ray_t rotated_r = ray_create(&origin, &direction, r->time);

    if (!rotate->object->hit(rotate->object, &rotated_r, ray, rec)) {
        return false;
    }

    rec->p = (point3){
        rotate->cos_theta * rec->p.x + rotate->sin_theta * rec->p.z,
        rec->p.y, -rotate->sin_theta * rec->p.x + rotate->cos_theta * rec->p.z
    };

    rec->normal = (vec3){
        rotate->cos_theta * rec->normal.x + rotate->sin_theta * rec->normal.z,
        rec->normal.y, -rotate->sin_theta * rec->normal.x + rotate->cos_theta * rec->normal.z
    };

    rec->front_face = vec3_dot(&rec->normal, &rotated_r.dir) < 0.0;
    return true;
}

bool rotate_y_bounding_box(const rotate_y_t* rotate, aabb_t* output_box) {
    if (!rotate->has_box) {
        return false;
    }

    *output_box = rotate->bbox;
    return true;
}
