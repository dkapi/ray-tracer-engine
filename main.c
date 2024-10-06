#include <stdint.h>
#include <stdbool.h>
#include "color.h"
#include "sphere.h"
#define X 1024
#define Y 1024

#define R 0
#define G 1
#define B 2

    //FIXME: normals are not being correctly computed, or color is off
color ray_color(const ray_t* r) 
{   
    point3 sphere_vecs = vec3_create_values(0,0,-1);
    sphere *s = sphere_create(&sphere_vecs, 0.5);
    vec3 unit_dir = vec3_unit_vector(&r->dir);
    double intersect = s->hit(s,r);
    if(intersect > 0.0) {
        vec3 ray_loc = ray_at(r, intersect);
        vec3 tmp = vec3_subtract_vec(&ray_loc, &s->center);
        vec3 N = vec3_unit_vector(&tmp);
        color col = {0.5 *(N.x + 1), 0.5 * (N.y +1), 0.5 * (N.z + 1)};
        return col;
    }
    //if blending factor  error occurs, just fix a calculation
    double a = 0.5 * (1 + unit_dir.y);
    color white = {1.0,1.0,1.0};
    color blue = {0.7, 0.5, 1.0};

    color blending_color;
    blending_color.x = (1.0 - a) * white.x + a * blue.x;
    blending_color.y = (1.0 - a) * white.y + a * blue.y;
    blending_color.z = (1.0 - a) * white.z + a * blue.z;
    return blending_color;
}


int main(int argc, char *argv[])
{
    FILE *img;
    int x, y;


    //viewport/camera things
    //eventually port to own files
    double aspect_ratio = 16.0/9.0; //ideal ratio
    int image_width = 1024;
    printf("aspect ratio: %f\n", aspect_ratio);

    int image_height = (int) (image_width / aspect_ratio);

    image_height = (image_height < 1) ? 1 : image_height;
    printf("image_height: %d\n", image_height);
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height  * ((double) image_width / image_height); 
    printf("viewport_width:%f\n", viewport_width);
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
    //fixed area, set delta_u scalar to height
    // fixed area, set delta_v scalar to width
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




    img = fopen("image.ppm", "w");
    fprintf(img, "P3\n%d\n%d\n255\n",image_width, image_height);

    for (y = 0; y < image_height; y++) {
        for(x = 0; x < image_width; x++) {
            vec3 temp_delta_u = vec3_multiply_by_scalar(&pixel_delta_u, x);
            vec3 temp_delta_v = vec3_multiply_by_scalar(&pixel_delta_v, y); 

            // Calculate the pixel center using the temporary variables
            vec3 pixel_center = vec3_add_vec(&pixel00_loc, &temp_delta_u);
            pixel_center = vec3_add_vec(&pixel_center, &temp_delta_v);
            

            //calculate ray direction
            vec3 ray_direction = vec3_subtract_vec(&pixel_center, &camera_center);
            ray_direction = vec3_unit_vector(&ray_direction); // Normalize 
            ray_t r = ray_create(&camera_center, &ray_direction);

            color pixel_color = ray_color(&r);
            write_color(img, pixel_color);
        }
    }
    
    return 0;

}