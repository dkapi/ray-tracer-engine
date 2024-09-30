#include <stdint.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"

#define X 1024
#define Y 1024

#define R 0
#define G 1
#define B 2


color ray_color(const ray_t* r) 
{
    vec3 unit_dir = vec3_unit_vector(&r->dir);
    //blennding factor
    double a = 0.5 * unit_dir.y + 1;
    color white = {0,0,0};
    color blue = {0.5, 0.7, 1.0};

    color ret;
    ret.x = (1.0 - a) * white.x + a * blue.x;
    ret.y = (1.0 - a) * white.y + a * blue.y;
    ret.z = (1.0 - a) * white.z + a * blue.z;
    return ret;
}


int main(int argc, char *argv[])
{
    FILE *img;
    int x, y;


    //viewport/camera things
    //eventually port to own files
    double aspect_ratio = 16.0/9.0;
    int image_width = 400;

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




    img = fopen("image.ppm", "w");
    fprintf(img, "P3\n%d\n%d\n255\n",image_width   , image_height);

    for (y = 0; y < image_height; y++) {
        for(x = 0; x < image_width; x++) {
            vec3 temp_delta_u = vec3_multiply_by_scalar(&pixel_delta_u, y);
            vec3 temp_delta_v = vec3_multiply_by_scalar(&pixel_delta_v, x); 

            // Calculate the pixel center using the temporary variables
            vec3 pixel_center = vec3_add_vec(&pixel00_loc, &temp_delta_u);
            pixel_center = vec3_add_vec(&pixel_center, &temp_delta_v);

            //calculate ray direction
            vec3 ray_direction = vec3_subtract_vec(&pixel_center, &camera_center);
            ray_t r = ray_create(&camera_center, &ray_direction);

            color pixel_color = ray_color(&r);
            write_color(img, pixel_color);
        }
    }
    
    return 0;

}