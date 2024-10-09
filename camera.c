#include "camera.h"

camera_t* camera_initialize()
{
    camera_t* c = (camera_t*)malloc(sizeof(camera_t));
    c->aspect_ratio = ASPECT_RATIO;
    c->image_width = I_WIDTH;
    int image_height = (int) I_WIDTH / (ASPECT_RATIO);
    c->image_height = image_height;
    c->focal_length = FOCAL_LENGHT;
    c->viewport_height = VIEWPORT_HEIGHT;
    double viewport_h = VIEWPORT_HEIGHT * (double) I_WIDTH / (I_HEIGHT);
    c->viewport_width = viewport_h;
    c->camera_center = vec3_create_values(0,0,0);
    c->viewport_u = vec3_create_values(c->viewport_width, 0, 0);
    c->viewport_v = vec3_create_values(0, -c->viewport_height, 0);

    //calc the horizontal and vertical delta vectors from pixel to pixel
    vec3 pixel_delta_u = vec3_divide_by_scalar(&c->viewport_u, (I_WIDTH));
    vec3 pixel_delta_v = vec3_divide_by_scalar(&c->viewport_v, I_HEIGHT);
    
    
    //calc the location of upper left pixel
    vec3 focal_length_vec = vec3_create_values(0, 0, FOCAL_LENGHT);
    vec3 temp = vec3_subtract_vec(&c->camera_center, &focal_length_vec);

    vec3 half_viewport_u = vec3_multiply_by_scalar(&c->viewport_u, 0.5);
    vec3 half_viewport_v = vec3_multiply_by_scalar(&c->viewport_v, 0.5);
    vec3 viewport_upper_left = vec3_subtract_vec(&temp, &half_viewport_u);
    viewport_upper_left = vec3_subtract_vec(&viewport_upper_left, &half_viewport_v);

    vec3 pixel_delta_sum = vec3_add_vec(&pixel_delta_u, &pixel_delta_v);
    vec3 half_pixel_delta = vec3_multiply_by_scalar(&pixel_delta_sum, 0.5);
    vec3 pixel00_loc = vec3_add_vec(&viewport_upper_left, &half_pixel_delta);

    c->pixel_delta_u = pixel_delta_u;
    c->pixel_delta_v = pixel_delta_v;
    c->pixel_00_loc = pixel00_loc;

    return c;
}


// ray color function
color ray_color(const ray_t *r, const hittable_list *world) {
    hit_record_t rec;
    if (hittable_list_hit(world, r, interval_create(0, INFINITY), &rec)) {
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


void render(const camera_t* camera, hittable_list* world, FILE* img)
{
    int x, y;
    for (y = 0 ; y < camera->image_height; y++) { // Loop from top to bottom
        for (x = 0; x < camera->image_width; ++x) {
            // calculate pixel location and ray direction
            vec3 pixel_loc = camera->pixel_00_loc;
            // if image gets messed up, usually here
            vec3 i_scaled = vec3_multiply_by_scalar(&camera->pixel_delta_u, x);
            vec3 j_scaled = vec3_multiply_by_scalar(&camera->pixel_delta_v, y);
            pixel_loc = vec3_add_vec(&camera->pixel_00_loc, &i_scaled);
            pixel_loc = vec3_add_vec(&pixel_loc, &j_scaled);

            // Compute the ray direction
            vec3 ray_dir = vec3_subtract_vec(&pixel_loc, &camera->camera_center);
            ray_t r = ray_create(&camera->camera_center, &ray_dir);

            // Get the color of the ray
            color pixel_color = ray_color(&r, world);
            write_color(img, pixel_color);

        }
    }


}