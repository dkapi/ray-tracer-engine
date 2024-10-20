#include "camera.h"

void camera_initialize(camera_t* c) {
    // Ensure image height calculation is proper and avoid unintended integer division.
    c->image_height = (int)(c->image_width / (c->aspect_ratio));
    c->image_height = (c->image_height < 1) ? 1 : c->image_height;

    // Set up viewport dimensions
    c->viewport_height = 2.0;
    c->viewport_width  = c->viewport_height * (double) c->image_width / c->image_height;

    // Set the scale factor for samples
    if (c->samples_per_pixel <= 0) {
        c->samples_per_pixel = 1; 
    }
    c->pixel_samples_scale = 1.0 / c->samples_per_pixel;

    // Set camera properties
    c->camera_center = vec3_create_values(0, 0, 0);
    c->viewport_u = vec3_create_values(c->viewport_width, 0, 0);
    c->viewport_v = vec3_create_values(0, -c->viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel
    c->pixel_delta_u = vec3_divide_by_scalar(&c->viewport_u, c->image_width);
    c->pixel_delta_v = vec3_divide_by_scalar(&c->viewport_v, c->image_height);

    // Calculate the location of the upper left pixel
    vec3 focal_length_vec = vec3_create_values(0, 0, c->focal_length);
    vec3 temp = vec3_subtract(&c->camera_center, &focal_length_vec);

    vec3 half_viewport_u = vec3_multiply_by_scalar(&c->viewport_u, 0.5);
    vec3 half_viewport_v = vec3_multiply_by_scalar(&c->viewport_v, 0.5);
    vec3 viewport_upper_left = vec3_subtract(&temp, &half_viewport_u);
    viewport_upper_left = vec3_subtract(&viewport_upper_left, &half_viewport_v);

    vec3 pixel_delta_sum = vec3_add(&c->pixel_delta_u, &c->pixel_delta_v);
    vec3 half_pixel_delta = vec3_multiply_by_scalar(&pixel_delta_sum, 0.5);
    c->pixel_00_loc = vec3_add(&viewport_upper_left, &half_pixel_delta);
}

    // returns a random real in [0,1).
static inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

// ray color function
color ray_color(const ray_t *r, const hittable_list *world) {
    hit_record_t rec;
    
    // Check if the ray hits anything in the world
    if (hittable_list_hit(world, r, interval_create(0.001, INFINITY), &rec)) {
        // Shift normal to the [0, 1] range for coloring
        vec3 normal_color = vec3_create_values(rec.normal.x + 1.0, rec.normal.y + 1.0, rec.normal.z + 1.0);
        return vec3_multiply_by_scalar(&normal_color, 0.5);
    }

    // background gradient 
    vec3 unit_direction = vec3_unit_vector(&r->dir);
    double t = 0.5 * (unit_direction.y + 1.0);
    
    color white = vec3_create_values(1.0, 1.0, 1.0);
    color blue = vec3_create_values(0.7, 0.5, 1.0);

    // Linear blend between white and blue based on t for background
    color white_scaled = vec3_multiply_by_scalar(&white, 1.0 - t);
    color blue_scaled = vec3_multiply_by_scalar(&blue, t);
    color blended = vec3_add(&white_scaled, &blue_scaled);
    return blended;
}


// returns the vector to a random point in the [-.5, -.5]-[+.5, +.5] unit square.
// for anti-aliasing
vec3 sample_square() {
    return vec3_create_values(random_double() - 0.5, random_double() - 0.5, 0.0);
}

// constructs a ray originating from the camera center and directed at a sampled point near pixel i, j
ray_t get_ray(const camera_t* camera, int i, int j) {
    vec3 offset = sample_square();

    // calculate the sampled pixel location
    vec3 i_scaled = vec3_multiply_by_scalar(&camera->pixel_delta_u, (double)(i) + offset.x);
    vec3 j_scaled = vec3_multiply_by_scalar(&camera->pixel_delta_v, (double)(j) + offset.y);
    vec3 pixel_sample = vec3_add(&camera->pixel_00_loc, &i_scaled);
    pixel_sample = vec3_add(&pixel_sample, &j_scaled);

    // compute the ray direction
    vec3 ray_direction = vec3_subtract(&pixel_sample, &camera->camera_center);
    return ray_create(&camera->camera_center, &ray_direction);
}

void display_progress(int completed, int total) {
    int bar_width = 70;

    double progress = (double)completed / (double)total;

    printf("\r[");
    int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) {
            printf("=");
        } else if (i == pos) {
            printf(">");
        } else {
            printf(" ");
        }
    }
    printf("] %d%%", (int)(progress * 100));

    fflush(stdout);
}

void render(const camera_t* camera, hittable_list* world, FILE* img) {
    int x, y, sample;
    int total_pixels = camera->image_height * camera->image_width;
    fprintf(img, "P6\n%d %d\n255\n", camera->image_width, camera->image_height);

    for (y = 0; y < camera->image_height; y++) {
        for (x = 0; x < camera->image_width; x++) {
            // accumulate color for each pixel by sampling multiple times
            color pixel_color = vec3_create_values(0, 0, 0);
            for (sample = 0; sample < camera->samples_per_pixel; ++sample) {
                ray_t r = get_ray(camera, x, y);

                color ray_col = ray_color(&r, world);
                pixel_color = vec3_add(&pixel_color, &ray_col);
            }

            pixel_color = vec3_multiply_by_scalar(&pixel_color, camera->pixel_samples_scale);
            write_color(img, pixel_color);

            //progress bar
            int completed_pixels = (y * camera->image_width) + x + 1;
            if (completed_pixels % (total_pixels / 100) == 0) {
            display_progress(completed_pixels, total_pixels);
            }
        }
    }
    display_progress(total_pixels, total_pixels);
    printf("\n");
}