#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "triangle_mesh.h"

#define MAX_LINE_LENGTH 256 //should be fine, if anything too much

mesh_t* load_obj(const char* filename, material_t* mat) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "failed to open OBJ file: %s\n", filename);
        return NULL;
    }

    // temporary storage, idk maybe make more dynamic, works for now
    point3* vertices = (point3*)malloc(10000 * sizeof(point3));
    vec3* texture_coords = (vec3*)malloc(10000 * sizeof(vec3));
    triangle_t** triangles = (triangle_t**)malloc(10000 * sizeof(triangle_t*));

    size_t vertex_count = 0;
    size_t texcoord_count = 0;
    size_t triangle_count = 0;

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0) {
            //vertex position
            double x, y, z;
            sscanf(line + 2, "%lf %lf %lf", &x, &y, &z);
            vertices[vertex_count++] = vec3_create_values(x, y, z);
        } else if (strncmp(line, "vt ", 3) == 0) {
            //texture coordinate
            double u, v;
            sscanf(line + 3, "%lf %lf", &u, &v);
            texture_coords[texcoord_count++] = vec3_create_values(u, v, 0.0);
        } else if (strncmp(line, "f ", 2) == 0) {
            //face
            int v[4], vt[4], vn[4];
            int num_vertices = sscanf(
                line + 2,
                "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                &v[0], &vt[0], &vn[0],
                &v[1], &vt[1], &vn[1],
                &v[2], &vt[2], &vn[2],
                &v[3], &vt[3], &vn[3]
            );

            if (num_vertices < 9) {
                fprintf(stderr, "invalid face format: %s\n", line);
                continue;
            }

            point3* a = &vertices[v[0] - 1];
            point3* b = &vertices[v[1] - 1];
            point3* c = &vertices[v[2] - 1];

            vec3 default_uv = vec3_create_values(0.0, 0.0, 0.0);
            vec3* uv_a = (vt[0] > 0) ? &texture_coords[vt[0] - 1] : &default_uv;
            vec3* uv_b = (vt[1] > 0) ? &texture_coords[vt[1] - 1] : &default_uv;
            vec3* uv_c = (vt[2] > 0) ? &texture_coords[vt[2] - 1] : &default_uv;

            triangles[triangle_count++] = create_triangle_uv(a, b, c, uv_a, uv_b, uv_c, mat);

            // handling quads
            if (num_vertices == 12) {
                point3* d = &vertices[v[3] - 1];
                vec3* uv_d = (vt[3] > 0) ? &texture_coords[vt[3] - 1] : &default_uv;

                triangles[triangle_count++] = create_triangle_uv(a, c, d, uv_a, uv_c, uv_d, mat);
            }
        }
    }

    fclose(file);

    // allocate mesh
    mesh_t* mesh = (mesh_t*)malloc(sizeof(mesh_t));
    mesh->vertices = (point3*)malloc(vertex_count * sizeof(point3));
    memcpy(mesh->vertices, vertices, vertex_count * sizeof(point3));

    mesh->texture_coords = (vec3*)malloc(texcoord_count * sizeof(vec3));
    memcpy(mesh->texture_coords, texture_coords, texcoord_count * sizeof(vec3));

    mesh->triangles = (triangle_t**)malloc(triangle_count * sizeof(triangle_t*));
    memcpy(mesh->triangles, triangles, triangle_count * sizeof(triangle_t*));

    mesh->vertex_count = vertex_count;
    mesh->texcoord_count = texcoord_count;
    mesh->triangle_count = triangle_count;

    free(vertices);
    free(texture_coords);
    free(triangles);

    return mesh;
}

void add_mesh_to_world(mesh_t* mesh, hittable_list* world) {
    for (size_t i = 0; i < mesh->triangle_count; i++) {
        hittable_list_add(world, (hittable*)mesh->triangles[i]);
    }
}

void free_mesh(mesh_t* mesh) {
    if (!mesh) return;

    for (size_t i = 0; i < mesh->triangle_count; i++) {
        free(mesh->triangles[i]);
    }

    free(mesh->triangles);
    free(mesh->vertices);
    free(mesh->texture_coords);
    free(mesh);
}