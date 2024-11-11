#include "bvh.h"
#include "stdlib.h"

bvh_node_t* bvh_node_create(hittable_list* list) {
    return bvh_node_init((hittable **)list->objects->data, 0, list->objects->count);
}

static inline int random_int(int min, int max) {
    return ((int)random_double(min, max+1));
}

int box_x_compare(const void *a, const void *b) {
    hittable *hittable_a = *(hittable **)a;
    hittable *hittable_b = *(hittable **)b;

    aabb_t box_a, box_b;

    if (!hittable_a->bbox(hittable_a, &box_a) || !hittable_b->bbox(hittable_b, &box_b)) {
        return 0;  // if either object doesn't have a bounding box, we can't compare them
    }

    return (box_a.x.min < box_b.x.min) ? -1 : 1;
}

int box_y_compare(const void *a, const void *b) {
    hittable *hittable_a = *(hittable **)a;
    hittable *hittable_b = *(hittable **)b;

    aabb_t box_a, box_b;

    if (!hittable_a->bbox(hittable_a, &box_a) || !hittable_b->bbox(hittable_b, &box_b)) {
        return 0;  // if either object doesn't have a bounding box, we can't compare them
    }

    return (box_a.y.min < box_b.y.min) ? -1 : 1;
}

int box_z_compare(const void *a, const void *b) {
    hittable *hittable_a = *(hittable **)a;
    hittable *hittable_b = *(hittable **)b;

    aabb_t box_a, box_b;

    if (!hittable_a->bbox(hittable_a, &box_a) || !hittable_b->bbox(hittable_b, &box_b)) {
        return 0;  // if either object doesn't have a bounding box, we can't compare them
    }

    return (box_a.z.min < box_b.z.min) ? -1 : 1;
}

// function to determine the longest axis of a bounding box
static int bbox_longest_axis(const aabb_t *box) {
    double x_size = box->x.max - box->x.min;
    double y_size = box->y.max - box->y.min;
    double z_size = box->z.max - box->z.min;

    if (x_size > y_size && x_size > z_size) {
        return 0;
    } else if (y_size > z_size) {
        return 1;
    } else {
        return 2;
    }
}


bvh_node_t* bvh_node_init(hittable **objects, size_t start, size_t end) {
    bvh_node_t *node = (bvh_node_t *)malloc(sizeof(bvh_node_t));


    // Calculate the bounding box of all objects in this node
    aabb_t bbox = aabb_create_empty();
    for (size_t i = start; i < end; ++i) {
        aabb_t temp_box;
        bbox = aabb_create_with_boxes(&bbox, &temp_box);
    }

    node->bbox = bbox;

    // choose a based on logest axis
    int axis = bbox_longest_axis(&bbox);
    size_t object_span = end - start;

    if (object_span == 1) {
        // only one object, point left and right to it
        node->left = objects[start];
        node->right = objects[start];  // Point to the same object to avoid NULL
    } else if (object_span == 2) {
        // two objects, determine left and right based on their bounding boxes
        if (box_x_compare(&objects[start], &objects[start + 1]) < 0) {
            node->left = objects[start];
            node->right = objects[start + 1];
        } else {
            node->left = objects[start + 1];
            node->right = objects[start];
        }
    } else {
        // more than two objects, sort them based on the chosen axis
        if (axis == 0) {
            qsort(objects + start, object_span, sizeof(hittable *), box_x_compare);
        } else if (axis == 1) {
            qsort(objects + start, object_span, sizeof(hittable *), box_y_compare);
        } else {
            qsort(objects + start, object_span, sizeof(hittable *), box_z_compare);
        }

        // split the list into two halves and create BVH nodes for each half
        size_t mid = start + object_span / 2;

        node->left  = (hittable *)bvh_node_init(objects, start, mid);
        node->right = (hittable *)bvh_node_init(objects, mid, end);

    }

    // calculate the bounding box for this node
    aabb_t box_left, box_right;

    // If right is the same as left (single object case), box_right is not needed
    node->bbox = (node->left == node->right) ? box_left : aabb_surrounding_box(&box_left, &box_right);

    // assign function pointers
    node->base.hit = (hit_fn)hit_bvh_node;
    node->base.bbox = (bounding_box_fn)bounding_box_bvh_node;

    return node;
}


// BVH hit function
bool hit_bvh_node(const bvh_node_t *node, const ray_t *r, interval_t ray_interval, hit_record_t *rec) {
    if (!aabb_hit(&node->bbox, r, ray_interval)) {
        return false; 
    }

    bool hit_left = false;
    bool hit_right = false;
    hit_record_t temp_rec;

    if (node->left) {
        hit_left = node->left->hit(node->left, r, ray_interval, &temp_rec);
        if (hit_left) {
            ray_interval.max = temp_rec.t;
            *rec = temp_rec;
        }
    }

    if (node->right) {
        hit_right = node->right->hit(node->right, r, ray_interval, &temp_rec);
        if (hit_right) {
            *rec = temp_rec;
        }
    }

    return hit_left || hit_right;
}


// BVH bounding box function
bool bounding_box_bvh_node(const bvh_node_t *node, aabb_t *output_box) {
    *output_box = node->bbox;
    return true;
}

// helper function for comparing bounding boxes based on a specific axis
int compare_bounding_boxes(const void *a, const void *b, int axis) {
    aabb_t box_a, box_b;

    hittable *hittable_a = *(hittable **)a;
    hittable *hittable_b = *(hittable **)b;

    if (!hittable_a->bbox(hittable_a, &box_a) || !hittable_b->bbox(hittable_b, &box_b)) {
        return 0;
    }

    double a_min = (axis == 0) ? box_a.x.min : (axis == 1) ? box_a.y.min : box_a.z.min;
    double b_min = (axis == 0) ? box_b.x.min : (axis == 1) ? box_b.y.min : box_b.z.min;

    return (a_min < b_min) ? -1 : 1;
}

void bvh_node_free(bvh_node_t *node) {
    if (!node) return;

    // free left and right
    if (node->left && node->left->hit == (hit_fn)hit_bvh_node) {
        bvh_node_free((bvh_node_t *)node->left);
    }
    if (node->right && node->right->hit == (hit_fn)hit_bvh_node) {
        bvh_node_free((bvh_node_t *)node->right);
    }

    free(node);
}


// helper to print bounding box
static void print_aabb(const aabb_t *box) {
    printf("Bounding Box: X[min=%.2f, max=%.2f], Y[min=%.2f, max=%.2f], Z[min=%.2f, max=%.2f]\n",
           box->x.min, box->x.max, box->y.min, box->y.max, box->z.min, box->z.max);
}

// recursive function to traverse and print bvh
void print_bvh_tree(const bvh_node_t *node, int depth) {
    if (node == NULL) {
        printf("%*s<NULL NODE>\n", depth * 2, "");
        return;
    }

    printf("%*sNode at depth %d:\n", depth * 2, "", depth);
    print_aabb(&node->bbox);

    if (node->left != NULL) {
        printf("%*sLeft child:\n", depth * 2, "");
        if (node->left->hit == (hit_fn)hit_bvh_node) {
            print_bvh_tree((bvh_node_t *)node->left, depth + 1);
        } else {
            printf("%*s<Leaf Node>\n", (depth + 1) * 2, "");
        }
    } else {
        printf("%*sNo left child\n", depth * 2, "");
    }

    if (node->right != NULL) {
        printf("%*sRight child:\n", depth * 2, "");
        if (node->right->hit == (hit_fn)hit_bvh_node) {
            print_bvh_tree((bvh_node_t *)node->right, depth + 1);
        } else {
            printf("%*s<Leaf Node>\n", (depth + 1) * 2, "");
        }
    } else {
        printf("%*sNo right child\n", depth * 2, "");
    }
}