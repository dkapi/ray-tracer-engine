#include "camera.h"

static inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

void camera_initialize(camera_t* c) {
    c->image_height = (int)(c->image_width / (c->aspect_ratio));
    c->image_height = (c->image_height < 1) ? 1 : c->image_height;

    // sample scale factor
    if (c->samples_per_pixel <= 0) {
        c->samples_per_pixel = 1; 
    }
    c->pixel_samples_scale = 1.0 / c->samples_per_pixel;

    // cam position and cam frame vectors
    c->camera_center = c->lookfrom;

    // Calculate focal length
    vec3 temp_sub = vec3_subtract(&c->lookfrom, &c->lookat);
    c->focal_length = vec3_length(&temp_sub);

    double theta = degrees_to_radians(c->vfov);
    double h = tan(theta / 2.0);

    // Set viewport dimensions based on focus distance
    c->viewport_height = 2 * h * c->focus_dist;
    c->viewport_width = c->viewport_height * ((double) c->image_width / c->image_height);

    // Calculate camera frame basis vectors
    c->w = vec3_unit_vector(&temp_sub);
    vec3 crosstmp = vec3_cross(&c->vup, &c->w);
    c->u = vec3_unit_vector(&crosstmp);
    c->v = vec3_cross(&c->w, &c->u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges
    c->viewport_u = vec3_multiply_by_scalar(&c->u, c->viewport_width);
    c->viewport_v = vec3_multiply_by_scalar(&c->v, c->viewport_height);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel
    c->pixel_delta_u = vec3_divide_by_scalar(&c->viewport_u, c->image_width);
    c->pixel_delta_v = vec3_divide_by_scalar(&c->viewport_v, c->image_height);

    // Calculate the location of the upper left pixel
    vec3 focal_length_vec = vec3_multiply_by_scalar(&c->w, c->focus_dist);
    vec3 viewport_upper_left = vec3_subtract(&c->camera_center, &focal_length_vec);
    vec3 divide_temp1 = vec3_divide_by_scalar(&c->viewport_u, 2);
    viewport_upper_left = vec3_subtract(&viewport_upper_left, &divide_temp1);
    vec3 divide_temp2 = vec3_divide_by_scalar(&c->viewport_v, 2);
    viewport_upper_left = vec3_subtract(&viewport_upper_left, &divide_temp2);

    vec3 pixel_delta_sum = vec3_add(&c->pixel_delta_u, &c->pixel_delta_v);
    vec3 half_pixel_delta = vec3_multiply_by_scalar(&pixel_delta_sum, 0.5);
    c->pixel_00_loc = vec3_add(&viewport_upper_left, &half_pixel_delta);

    // Calculate defocus disk basis vectors
    double defocus_radius = c->focus_dist * tan(degrees_to_radians(c->defocus_angle / 2.0));
    c->defocus_disk_u = vec3_multiply_by_scalar(&c->u, defocus_radius);
    c->defocus_disk_v = vec3_multiply_by_scalar(&c->v, defocus_radius);
} 

// ray color function
color ray_color(const ray_t *r, int depth ,const bvh_node_t* world) {
    //max depth check
    if(depth <= 0) {
        return vec3_create_values(0,0,0);
    }

    hit_record_t rec;
    
    // Check if the ray hits anything in the world
    if (hit_bvh_node(world, r, interval_create(0.001, INFINITY), &rec)) {
        ray_t scattered;
        color attenuation;

        if (rec.mat->scatter(rec.mat, r, &rec, &attenuation, &scattered)) {
            color scattered_color = ray_color(&scattered, depth -1, world);
            return vec3_multiply(&attenuation, &scattered_color);
        }

        return vec3_create_values(0, 0, 0);
    }

    // background gradient 
    vec3 unit_direction = vec3_unit_vector(&r->dir);
    double t = 0.5 * (unit_direction.y + 1.0);
    
    color white = vec3_create_values(1.0, 1.0, 1.0);
    color blue = vec3_create_values(0.7, 0.5, 1.0);

    // linear blend between white and blue based on t for background
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

static point3 defocus_disk_sample(const camera_t* camera) {
    vec3 p = random_in_unit_disk();

    vec3 scaled_u = vec3_multiply_by_scalar(&camera->defocus_disk_u, p.x);
    vec3 scaled_v = vec3_multiply_by_scalar(&camera->defocus_disk_v, p.y);

    vec3 offset = vec3_add(&scaled_u, &scaled_v);
    return vec3_add(&camera->camera_center, &offset);
}


// constructs a ray originating from the camera center and directed at a sampled point near pixel i, j
ray_t get_ray(const camera_t* camera, int i, int j) {
    vec3 offset = sample_square();

    // calculate the sampled pixel location
    vec3 i_scaled = vec3_multiply_by_scalar(&camera->pixel_delta_u, (double)(i) + offset.x);
    vec3 j_scaled = vec3_multiply_by_scalar(&camera->pixel_delta_v, (double)(j) + offset.y);
    vec3 pixel_sample = vec3_add(&camera->pixel_00_loc, &i_scaled);
    pixel_sample = vec3_add(&pixel_sample, &j_scaled);

    //depth of field
    vec3 ray_origin;
    if (camera->defocus_angle <= 0) {
        ray_origin = camera->camera_center;
    } else {
        ray_origin = defocus_disk_sample(camera);
    }
    vec3 ray_direction = vec3_subtract(&pixel_sample, &ray_origin);
    double ray_time = random_double();
    return ray_create(&ray_origin, &ray_direction, ray_time);
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


void render(const camera_t* camera, bvh_node_t* world, pixel_t** raster) {
    int total_pixels = camera->image_height * camera->image_width;
    int completed_pixels = 0;

    // render loop with added omp parallelization
    #pragma omp parallel for schedule(dynamic) collapse(2)
    for (int y = camera->image_height - 1; y >= 0; y--) {
        for (int x = 0; x < camera->image_width; x++) {
            // accumulate color for each pixel by sampling multiple times
            color pixel_color = vec3_create_values(0, 0, 0);
            for (int sample = 0; sample < camera->samples_per_pixel; ++sample) {
                ray_t r = get_ray(camera, x, y);
                color ray_col = ray_color(&r, camera->max_depth, world);
                pixel_color = vec3_add(&pixel_color, &ray_col);
            }

            // scale the accumulated color for this pixel
            pixel_color = vec3_multiply_by_scalar(&pixel_color, camera->pixel_samples_scale);

            // write the final color to the raster
            write_color(&raster[y][x], pixel_color);

            // Progress bar update
            //atomic flag since each thread has access to completed_pixels variable
            #pragma omp atomic
            completed_pixels++;

            // updating progress bar 
            if (completed_pixels % (total_pixels / 100) == 0) {
                #pragma omp critical
                display_progress(completed_pixels, total_pixels);
            }
        }
    }
    display_progress(total_pixels, total_pixels);
    printf("\n");
}
