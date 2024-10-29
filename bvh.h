#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "hittable.h"


typedef struct bvh_node {
    hittable base;            // Base hittable structure, must be first
    hittable *left;           // Pointer to left child (another hittable)
    hittable *right;          // Pointer to right child (another hittable)
    aabb_t bbox;              // Bounding box for this BVH node
} bvh_node_t;

// Function prototypes
bvh_node_t* bvh_node_create(hittable_list *list);
bvh_node_t* bvh_node_create_with_objects(hittable **objects, size_t start, size_t end);
bool hit_bvh_node(const bvh_node_t *node, const ray_t *r, interval_t ray_interval, hit_record_t *rec);
bool bounding_box_bvh_node(const bvh_node_t *node, aabb_t *output_box);
void bvh_node_free(bvh_node_t *node);
void print_bvh_tree(const bvh_node_t *node, int depth);

#endif /* BVH_H */