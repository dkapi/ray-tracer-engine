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

// TODO: make cubemap work from a single cross image
// // create a cube map from a single image with a given layout
// cubemap_t* create_cubemap_from_single_image(const char* filepath, int layout) {
//     int width, height, channels;
//     unsigned char* data = stbi_load(filepath, &width, &height, &channels, 3);
//     if (!data) {
//         fprintf(stderr, "Failed to load cube map image: %s\n", filepath);
//         return NULL;
//     }

//     int face_size;
//     if (layout == CUBEMAP_LAYOUT_CROSS) {
//         face_size = width / 4;
//         if (face_size * 3 != height) {
//             fprintf(stderr, "Invalid cube map cross layout dimensions.\n");
//             stbi_image_free(data);
//             return NULL;
//         }
//     } else if (layout == CUBEMAP_LAYOUT_HORIZONTAL) {
//         face_size = height;
//         if (width != face_size * 6) {
//             fprintf(stderr, "Invalid cube map horizontal strip dimensions.\n");
//             stbi_image_free(data);
//             return NULL;
//         }
//     } else {
//         fprintf(stderr, "Unsupported cube map layout.\n");
//         stbi_image_free(data);
//         return NULL;
//     }

//     cubemap_t* cubemap = (cubemap_t*)malloc(sizeof(cubemap_t));
//     if (!cubemap) {
//         fprintf(stderr, "Failed to allocate memory for cube map.\n");
//         stbi_image_free(data);
//         return NULL;
//     }

//     for (int i = 0; i < 6; i++) {
//         int x_offset = 0, y_offset = 0;

//         if (layout == CUBEMAP_LAYOUT_CROSS) {
//             int offsets[6][2] = {
//                 {2, 1}, {0, 1}, {1, 0}, {1, 2}, {1, 1}, {3, 1}
//             };
//             x_offset = offsets[i][0] * face_size;
//             y_offset = offsets[i][1] * face_size;
//         } else if (layout == CUBEMAP_LAYOUT_HORIZONTAL) {
//             x_offset = i * face_size;
//         }

//         unsigned char* face_data = (unsigned char*)malloc(face_size * face_size * 3);
//         if (!face_data) {
//             fprintf(stderr, "Failed to allocate memory for cube map face.\n");
//             destroy_cubemap(cubemap);
//             stbi_image_free(data);
//             return NULL;
//         }

//         for (int y = 0; y < face_size; y++) {
//             memcpy(&face_data[y * face_size * 3],
//                    &data[((y + y_offset) * width + x_offset) * 3],
//                    face_size * 3);
//         }

//         cubemap->faces[i] = (texture_t*)create_image_texture_from_data(face_data, face_size, face_size, 3);
//         free(face_data);
//     }

//     stbi_image_free(data);
//     return cubemap;
// }

//sample the cubemap
color sample_cubemap(const cubemap_t* cubemap, const vec3* direction) {
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