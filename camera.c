#include "camera.h"


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
color ray_color(const ray_t *r, int depth, const bvh_node_t *world, hittable_list *lights, const color *background, const cubemap_t *cubemap, const hdr_texture_t *hdr) {
    // Base case: terminate recursion
    if (depth <= 0) {
        return vec3_create_values(0, 0, 0);
    }

    hit_record_t rec;

    // If the ray misses the world, return background
    if (!hit_bvh_node(world, r, interval_create(0.001, INFINITY), &rec)) {
        if (cubemap) {
            vec3 dir = vec3_unit_vector(&r->dir);
            return sample_cubemap(cubemap, &dir);
        }
        if (hdr) {
            vec3 dir = vec3_unit_vector(&r->dir);
            double u, v;
            direction_to_uv(&dir, &u, &v);
            return sample_hdr(hdr, u, v);
        }
        return *background;
    }

    // Emitted light
    color color_from_emission = (rec.mat && rec.mat->emitted) 
        ? rec.mat->emitted(rec.mat, r, &rec, rec.u, rec.v, &rec.p)
        : vec3_create_values(0, 0, 0);

    // If scattering fails, return emitted light only
    scatter_record_t srec;
    if (!rec.mat || !rec.mat->scatter(rec.mat, r, &rec, &srec)) {
        return color_from_emission;
    }

    // Handle skip_pdf case
    if (srec.skip_pdf) {
        color ray_color_returned = ray_color(&srec.skip_pdf_ray, depth - 1, world, lights, background, cubemap, hdr);
        return vec3_multiply(&srec.attenuation, &ray_color_returned);
    }

    // Create a mixture PDF with the lights and material scattering PDF
    pdf_t* light_pdf = hittable_pdf_create((const hittable*)&lights->base, &rec.p);
    pdf_t* mixture_pdf = (pdf_t*)mixture_pdf_create(light_pdf, srec.pdf_ptr);

    // Generate a scattered direction using the mixture PDF
    vec3 scatter_direction = mixture_pdf->generate(mixture_pdf);
    ray_t scattered = ray_create(&rec.p, &scatter_direction, r->time);
scatter_direction = vec3_unit_vector(&scatter_direction);

// Validate the scattered direction
if (isnan(scatter_direction.x) || isnan(scatter_direction.y) || isnan(scatter_direction.z)) {
    return vec3_create_values(0, 0, 0); // Fallback to black
}

// Compute the PDF value
double pdf_value = mixture_pdf->value(mixture_pdf, &scatter_direction);
if (pdf_value <= 0 || isnan(pdf_value)) {
    pdf_value = 1e-8; // Avoid invalid or zero PDF
}

// Scattering PDF
double scattering_pdf = rec.mat->scattering_pdf 
    ? rec.mat->scattering_pdf(rec.mat, r, &rec, &scattered) 
    : pdf_value;

// Ensure scattering_pdf is valid
if (scattering_pdf <= 0 || isnan(scattering_pdf)) {
    scattering_pdf = 1e-8; // Avoid invalid scattering PDF
}

// Recursive ray trace
color sample_color = ray_color(&scattered, depth - 1, world, lights, background, cubemap, hdr);

// Calculate light contribution from scattering
color color_from_scatter = vec3_multiply_by_scalar(&sample_color, scattering_pdf / pdf_value);
color contribution = vec3_multiply(&srec.attenuation, &color_from_scatter);

// Add emitted and scattered light
return vec3_add(&color_from_emission, &contribution);
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


void render(const camera_t* camera, bvh_node_t* world, hittable_list* lights, pixel_t** raster, const cubemap_t* cubemap, const hdr_texture_t* hdr) {
    int total_pixels = camera->image_height * camera->image_width;
    int completed_pixels = 0;

    // render loop with omp parallelization
    #pragma omp parallel for schedule(dynamic) collapse(2)
    for (int y = camera->image_height - 1; y >= 0; y--) {
        for (int x = 0; x < camera->image_width; x++) {
            // accumulate color for each pixel by sampling multiple times
            color pixel_color = vec3_create_values(0, 0, 0);
            for (int sample = 0; sample < camera->samples_per_pixel; ++sample) {
                ray_t r = get_ray(camera, x, y);
                color ray_col = ray_color(&r, camera->max_depth, world, lights, &camera->background, cubemap, hdr);
                pixel_color = vec3_add(&pixel_color, &ray_col);
            }

            // scale the accumulated color for this pixel
            pixel_color = vec3_multiply_by_scalar(&pixel_color, camera->pixel_samples_scale);

            // write the final color to the raster
            write_color(&raster[y][x], pixel_color);

            // progress bar update
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
