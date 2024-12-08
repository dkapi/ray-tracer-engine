#include "hdr.h"


// Load an HDR image
hdr_texture_t* load_hdr_image(const char* filepath) {
    hdr_texture_t* hdr_tex = (hdr_texture_t*)malloc(sizeof(hdr_texture_t));

    // load the HDR image using stb_image
    stbi_set_flip_vertically_on_load(1); // Flip vertically to match OpenGL-style coordinates
    hdr_tex->data = stbi_loadf(filepath, &hdr_tex->width, &hdr_tex->height, &hdr_tex->channels, 3); // Force RGB
    if (!hdr_tex->data) {
        fprintf(stderr, "failed to load HDR image: %s\n", filepath);
        free(hdr_tex);
        return NULL;
    }

    return hdr_tex;
}


// bilinear sampling for HDR textures
color sample_hdr(const hdr_texture_t* hdr_tex, double u, double v) {
    u = fmin(fmax(u, 0.0), 1.0);
    v = fmin(fmax(v, 0.0), 1.0);

    // convert UV to pixel coordinates
    double px = u * (hdr_tex->width - 1);
    double py = v * (hdr_tex->height - 1);

    int x = (int)px;
    int y = (int)py;

    // bilinear interpolation
    double u_ratio = px - x;
    double v_ratio = py - y;

    int x1 = (x + 1 < hdr_tex->width) ? x + 1 : x;
    int y1 = (y + 1 < hdr_tex->height) ? y + 1 : y;

    color c00 = vec3_create_values(hdr_tex->data[(y * hdr_tex->width + x) * 3 + 0],
                                   hdr_tex->data[(y * hdr_tex->width + x) * 3 + 1],
                                   hdr_tex->data[(y * hdr_tex->width + x) * 3 + 2]);

    color c10 = vec3_create_values(hdr_tex->data[(y * hdr_tex->width + x1) * 3 + 0],
                                   hdr_tex->data[(y * hdr_tex->width + x1) * 3 + 1],
                                   hdr_tex->data[(y * hdr_tex->width + x1) * 3 + 2]);

    color c01 = vec3_create_values(hdr_tex->data[(y1 * hdr_tex->width + x) * 3 + 0],
                                   hdr_tex->data[(y1 * hdr_tex->width + x) * 3 + 1],
                                   hdr_tex->data[(y1 * hdr_tex->width + x) * 3 + 2]);

    color c11 = vec3_create_values(hdr_tex->data[(y1 * hdr_tex->width + x1) * 3 + 0],
                                   hdr_tex->data[(y1 * hdr_tex->width + x1) * 3 + 1],
                                   hdr_tex->data[(y1 * hdr_tex->width + x1) * 3 + 2]);

    vec3 c00_temp = vec3_multiply_by_scalar(&c00, 1 - u_ratio);
    vec3 c10_temp = vec3_multiply_by_scalar(&c10, u_ratio);
    color c0 = vec3_add(&c00_temp, &c10_temp);

    vec3 c01_temp = vec3_multiply_by_scalar(&c01, 1 - u_ratio);
    vec3 c11_temp = vec3_multiply_by_scalar(&c11, u_ratio);
    color c1 = vec3_add(&c01_temp, &c11_temp);

    vec3 c0_temp = vec3_multiply_by_scalar(&c0, 1 - v_ratio);
    vec3 c1_temp = vec3_multiply_by_scalar(&c1, v_ratio);

    return vec3_add(&c0_temp, &c1_temp);
}

void direction_to_uv(const vec3* dir, double* u, double* v) {
    double theta = atan2(dir->z, dir->x); // Longitude
    double phi = acos(fmax(-1.0, fmin(1.0, dir->y))); // Latitude, clamped for safety

    *u = (theta + PI) / (2.0 * PI); // Map theta to [0, 1]
    *v = 1.0 - (phi / PI);         // Map phi to [0, 1] and flip vertically
}

void free_hdr_texture(hdr_texture_t* hdr_tex) {
    if (hdr_tex) {
        if (hdr_tex->data) {
            stbi_image_free(hdr_tex->data);
        }
        free(hdr_tex);
    }
}
