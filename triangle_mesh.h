#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "vec3.h"
#include "triangle.h"
#include "hittable.h"

typedef struct {
    point3* vertices;
    vec3* texture_coords;
    triangle_t** triangles;
    size_t vertex_count;
    size_t texcoord_count;
    size_t triangle_count;
} mesh_t;

mesh_t* load_obj(const char* filename, material_t* mat);
void add_mesh_to_world(mesh_t* mesh, hittable_list* world);
void free_mesh(mesh_t* mesh);

#endif /* TRIANGLE_MESH_H */