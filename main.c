#include "ray.h"
#include "color.h"
#include "vec3.h"
#include "hittable.h"
#include "sphere.h"
#include <stdio.h>

// ray color function
color ray_color(const ray_t *r, const hittable_list *world) {
    hit_record_t rec;
    if (hittable_list_hit(world, r, 0, INFINITY, &rec)) {
        vec3 tmp = vec3_create_values(1, 1, 1);
        vec3 normal_color = vec3_add_vec(&tmp, &rec.normal);
        return vec3_multiply_by_scalar(&normal_color, 0.5);
    }

    vec3 unit_direction = vec3_unit_vector(&r->dir);
    double t = 0.5 * (unit_direction.y + 1.0);
    color white = vec3_create_values(1.0, 1.0, 1.0);
    color blue = vec3_create_values(0.7, 0.5, 1.0);
    color white_multi = vec3_multiply_by_scalar(&white, 1.0 - t);
    color purp_multi = vec3_multiply_by_scalar(&blue, t);
    return vec3_add_vec(&white_multi, &purp_multi);
}

int main() {

    // world
    hittable_list *world = hittable_list_create();
    
    // figure out how to call this in a const param? maybe u cant? not sure
    point3 center1 = vec3_create_values(0, 0, -1);
    point3 center2 = vec3_create_values(0, -100.5, -1);
    
    sphere *s1 = sphere_create(&center1, 0.5);
    sphere *s2 = sphere_create(&center2, 100);
    
    hittable_list_add(world, (hittable *)s1);
    hittable_list_add(world, (hittable *)s2);

    //viewport/camera things
    //eventually port to own files
    double aspect_ratio = 16.0/9.0;
    int image_width = 1024;

    int image_height = (int) image_width / aspect_ratio;
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height  * ((double) image_width / image_height); 
    point3 camera_center = {
        .x = 0,
        .y = 0,
        .z = 0
    };

    //calc vectors across the horizontal and down the vertical viewport edges
    vec3 viewport_u = {
        .x = viewport_width,
        .y = 0,
        .z = 0 
    };
    vec3 viewport_v = {
        .x = 0,
        .y = -viewport_height,
        .z = 0 
    };
    

    //calc the horizontal and vertical delta vectors from pixel to pixel
    vec3 pixel_delta_u = vec3_divide_by_scalar(&viewport_u, image_width);
    vec3 pixel_delta_v = vec3_divide_by_scalar(&viewport_v, image_height);
    
    
    //calc the location of upper left pixel
    vec3 focal_length_vec = vec3_create_values(0, 0, focal_length);
    vec3 temp = vec3_subtract_vec(&camera_center, &focal_length_vec);

    vec3 half_viewport_u = vec3_multiply_by_scalar(&viewport_u, 0.5);
    vec3 half_viewport_v = vec3_multiply_by_scalar(&viewport_v, 0.5);
    vec3 viewport_upper_left = vec3_subtract_vec(&temp, &half_viewport_u);
    viewport_upper_left = vec3_subtract_vec(&viewport_upper_left, &half_viewport_v);

    vec3 pixel_delta_sum = vec3_add_vec(&pixel_delta_u, &pixel_delta_v);
    vec3 half_pixel_delta = vec3_multiply_by_scalar(&pixel_delta_sum, 0.5);
    vec3 pixel00_loc = vec3_add_vec(&viewport_upper_left, &half_pixel_delta);

    // Open file to write PPM image & write header
    FILE *file = fopen("output.ppm", "w");
    fprintf(file, "P3\n%d %d\n255\n", image_width, image_height);

    // render loop
    int x, y;
    for (y = 0 ; y < image_height; y++) { // Loop from top to bottom
        for (x = 0; x < image_width; ++x) {
            // calculate pixel location and ray direction
            vec3 pixel_loc = pixel00_loc;
            // if image gets messed up, usually here
            vec3 i_scaled = vec3_multiply_by_scalar(&pixel_delta_u, x);
            vec3 j_scaled = vec3_multiply_by_scalar(&pixel_delta_v, y);
            pixel_loc = vec3_add_vec(&pixel00_loc, &i_scaled);
            pixel_loc = vec3_add_vec(&pixel_loc, &j_scaled);

            // Compute the ray direction
            vec3 ray_dir = vec3_subtract_vec(&pixel_loc, &camera_center);
            ray_t r = ray_create(&camera_center, &ray_dir);

            // Get the color of the ray
            color pixel_color = ray_color(&r, world);
            write_color(file, pixel_color);

        }
    }

    // Cleanup
    fclose(file);
    hittable_list_destroy(world);
    free(s1);
    free(s2);

    return 0;
}
