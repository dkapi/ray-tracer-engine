#include "engine.h"
#include <time.h>

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // Manually set the number of threads, else let OpenMP decide
    if (argc > 1) {
        int num_threads = atoi(argv[1]);
        if (num_threads > 0) {
            omp_set_num_threads(num_threads);
        }
    }

    // Camera setup
    camera_t camera = {0};
    camera.aspect_ratio      = 16.0 / 9.0;
    camera.image_width       = 1200;
    camera.samples_per_pixel = 500;
    camera.max_depth         = 50;
    camera.vfov              = 20;
    camera.lookfrom          = vec3_create_values(13, 2, 3);
    camera.lookat            = vec3_create_values(0, 0, 0);
    camera.vup               = vec3_create_values(0, 1, 0);
    camera.defocus_angle     = 0.6;
    camera.focus_dist        = 10.0;
    camera_initialize(&camera);

    // Image output
    FILE *img = fopen("output.ppm", "wb");
    
    // Raster
    pixel_t** raster = raster_init(camera.image_width, camera.image_height);

    // World
    hittable_list* world = hittable_list_create();

    texture_t* checker = (texture_t*)malloc(sizeof(checker_texture_t));
    color even_color = vec3_create_values(0.2, 0.3, 0.1); // Dark green
    color odd_color = vec3_create_values(0.9, 0.9, 0.9);  // Light gray
    checker = (texture_t*)create_checker_texture_solid(0.32, even_color, odd_color);

    // Ground sphere
    color ground_color = vec3_create_values(0.5, 0.5, 0.5);
    material_t *material_ground = (material_t *)create_lambertian_texture(checker);
    point3 ground_center = vec3_create_values(0.0, -1000.0, 0.0);
    sphere_t *ground_sphere = sphere_create(&ground_center, 1000.0, material_ground);
    hittable_list_add(world, (hittable *)ground_sphere);

    // Randomly scattered moving spheres
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            point3 center_start = vec3_create_values(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            point3 center_end = vec3_create_values(center_start.x, center_start.y + random_double() * 0.5, center_start.z);

            vec3 tmp_sub = vec3_subtract(&center_start, &(vec3){4, 0.2, 0});
            if (vec3_length(&tmp_sub) > 0.9) {
                material_t *sphere_material;

                if (choose_mat < 0.8) {
                    // Diffuse             
                    vec3 tmp1 = vec3_random();
                    vec3 tmp2 = vec3_random();
                    color albedo = vec3_multiply(&tmp1, &tmp2);
                    sphere_material = (material_t *)create_lambertian_color(&albedo);
                } else if (choose_mat < 0.95) {
                    // Metal
                    color albedo = vec3_random_values(0.5, 1.0);
                    double fuzz = random_double_in_range(0, 0.5);
                    sphere_material = (material_t *)create_metal(&albedo, fuzz);
                } else {
                    // Glass
                    sphere_material = (material_t *)create_dielectric(1.5);
                }

                moving_sphere_t *moving_sphere = moving_sphere_create(&center_start, &center_end, 0.2, 1.0, 0.0, sphere_material);
                hittable_list_add(world, (hittable *)moving_sphere);
            }
        }
    }

    // Three larger spheres
    // Glass sphere
    material_t *material1 = (material_t *)create_dielectric(1.5);
    point3 center1 = vec3_create_values(0.0, 1.0, 0.0);
    sphere_t *sphere1 = sphere_create(&center1, 1.0, material1);
    hittable_list_add(world, (hittable *)sphere1);

    // Lambertian sphere
    color center_color = vec3_create_values(0.4, 0.2, 0.1);
    material_t *material2 = (material_t *)create_lambertian_color(&center_color);
    point3 center2 = vec3_create_values(-4.0, 1.0, 0.0);
    sphere_t *sphere2 = sphere_create(&center2, 1.0, material2);
    hittable_list_add(world, (hittable *)sphere2);

    // Metal sphere
    color metal_color = vec3_create_values(0.7, 0.6, 0.5);
    material_t *material3 = (material_t *)create_metal(&metal_color, 0.0);
    point3 center3 = vec3_create_values(4.0, 1.0, 0.0);
    sphere_t *sphere3 = sphere_create(&center3, 1.0, material3);
    hittable_list_add(world, (hittable *)sphere3);

    // Create BVH from hittable list
    size_t object_count = darray_size(world->objects);
    hittable **objects = (hittable **)world->objects->data;

    bvh_node_t* bvh_world = bvh_node_init(objects, 0, object_count);
    if (!bvh_world) {
        fprintf(stderr, "Error: BVH creation failed\n");
        hittable_list_destroy(world);
        fclose(img);
        return 1;
    }

    // Render loop
    double start_time = omp_get_wtime();

    render(&camera, bvh_world, raster);
    raster_to_ppm(raster, camera.image_width, camera.image_height, img);

    double end_time = omp_get_wtime();

    double elapsed_time = end_time - start_time;
    int minutes = (int)(elapsed_time / 60);
    int seconds = (int)elapsed_time % 60;
    printf("\nRender completed in %d minutes and %d seconds.\n", minutes, seconds);

    // Cleanup
    fclose(img);
    hittable_list_destroy(world);
    bvh_node_free(bvh_world);
    free(ground_sphere);
    free(sphere1);
    free(sphere2);
    free(sphere3);

    return 0;
}
