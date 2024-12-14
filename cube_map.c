#include "cube_map.h"


cubemap_t* create_cubemap(const char* file_paths[6]) {
    cubemap_t* cubemap = (cubemap_t*)malloc(sizeof(cubemap_t));

    for (int i = 0; i < 6; i++) {
        cubemap->faces[i] = (texture_t*)create_image_texture(file_paths[i]);
        if (!cubemap->faces[i]) {
            fprintf(stderr, "failed to load cube map face: %s\n", file_paths[i]);
            destroy_cubemap(cubemap);
            return NULL;
        }
    }
    return cubemap;
}

//sample the cubemap
color cubemap_value(const cubemap_t* cubemap, const vec3* direction) {
    vec3 dir = vec3_unit_vector(direction);

    int face_index;
    double u, v;

    // determain which face 
    if (fabs(dir.x) > fabs(dir.y) && fabs(dir.x) > fabs(dir.z)) {
        face_index = (dir.x > 0) ? 0 : 1; // +X or -X
        u = (dir.z / fabs(dir.x) + 1.0) * 0.5;
        v = (dir.y / fabs(dir.x) + 1.0) * 0.5;
    } else if (fabs(dir.y) > fabs(dir.x) && fabs(dir.y) > fabs(dir.z)) {
        face_index = (dir.y > 0) ? 2 : 3; // +Y or -Y
        u = (dir.x / fabs(dir.y) + 1.0) * 0.5;
        v = (dir.z / fabs(dir.y) + 1.0) * 0.5;
    } else {
        face_index = (dir.z > 0) ? 4 : 5; // +Z or -Z
        u = (dir.x / fabs(dir.z) + 1.0) * 0.5;
        v = (dir.y / fabs(dir.z) + 1.0) * 0.5;
    }

    // idk why i have to do this, but without flipping these specific faces
    // the seams of the cube wouldnt align??
    if (face_index == 3 ) {
        v = 1.0 - v; // vertical flip
    }
    if ( face_index == 1 ) {
        u = 1.0 - u; // flip horizontal
    } 
    if ( face_index == 4 ) {
        u = 1.0 - u; // flip horizontal
    }

    u = fmin(fmax(u, 0.0), 1.0);
    v = fmin(fmax(v, 0.0), 1.0);

    return cubemap->faces[face_index]->value(cubemap->faces[face_index], u, v, NULL);
}


// dstroy the cube map
void destroy_cubemap(cubemap_t* cubemap) {
    if (cubemap) {
        for (int i = 0; i < 6; i++) {
            if (cubemap->faces[i]) {
                free_image_texture((image_texture_t*)cubemap->faces[i]);
            }
        }
        free(cubemap);
    }
}