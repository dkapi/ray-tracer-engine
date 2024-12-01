#include <time.h>
#include "engine.h"

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // manually set the number of threads, else let OpenMP decide
    if (argc > 1) {
        int num_threads = atoi(argv[1]);
        if (num_threads > 0) {
            omp_set_num_threads(num_threads);
        }
    }

    // Camera setup
    camera_t camera = {0};
    camera.aspect_ratio      = 1.0;
    camera.image_width       = 400;  
    camera.samples_per_pixel = 100; 
    camera.max_depth         = 50;
    camera.vfov              = 80;  
    camera.lookfrom          = vec3_create_values(0, 0, 9);
    camera.lookat            = vec3_create_values(0, 0, 0);
    camera.vup               = vec3_create_values(0, 1, 0);
    camera.defocus_angle     = 0.0; 
    camera.focus_dist        = 1.0;
    camera_initialize(&camera);

    // image output
    FILE *img = fopen("output.ppm", "wb");
    
    // raster
    pixel_t** raster = raster_init(camera.image_width, camera.image_height);

    // world
    hittable_list* world = hittable_list_create();

    material_t* left_red     = (material_t*)create_lambertian_color(&(color){1.0, 0.2, 0.2});
    material_t* back_green   = (material_t*)create_lambertian_color(&(color){0.2, 1.0, 0.2});
    material_t* right_blue   = (material_t*)create_lambertian_color(&(color){0.2, 0.2, 1.0});
    material_t* upper_orange = (material_t*)create_lambertian_color(&(color){1.0, 0.5, 0.0});
    material_t* lower_teal   = (material_t*)create_lambertian_color(&(color){0.2, 0.8, 0.8});

    // Quads
    quads_t* left_quad = quad_create(
        &(vec3){-3, -2, 5},  // Origin
        &(vec3){0, 0, -4},   // U vector
        &(vec3){0, 4, 0},    // V vector
        left_red
    );
    hittable_list_add(world, (hittable*)left_quad);

    quads_t* back_quad = quad_create(
        &(vec3){-2, -2, 0},
        &(vec3){4, 0, 0},
        &(vec3){0, 4, 0},
        back_green
    );
    hittable_list_add(world, (hittable*)back_quad);

    quads_t* right_quad = quad_create(
        &(vec3){3, -2, 1},
        &(vec3){0, 0, 4},
        &(vec3){0, 4, 0},
        right_blue
    );
    hittable_list_add(world, (hittable*)right_quad);

    quads_t* upper_quad = quad_create(
        &(vec3){-2, 3, 1},
        &(vec3){4, 0, 0},
        &(vec3){0, 0, 4},
        upper_orange
    );
    hittable_list_add(world, (hittable*)upper_quad);

    quads_t* lower_quad = quad_create(
        &(vec3){-2, -3, 5},
        &(vec3){4, 0, 0},
        &(vec3){0, 0, -4},
        lower_teal
    );
    hittable_list_add(world, (hittable*)lower_quad);
    // create BVH from hittable list
    size_t object_count = darray_size(world->objects);
    hittable **objects = (hittable **)world->objects->data;

    bvh_node_t* bvh_world = bvh_node_init(objects, 0, object_count);
    if (!bvh_world) {
        fprintf(stderr, "Error: BVH creation failed\n");
        hittable_list_destroy(world);
        fclose(img);
        return 1;
    }

    // render loop
    double start_time = omp_get_wtime();

    render(&camera, bvh_world, raster);
    raster_to_ppm(raster, camera.image_width, camera.image_height, img);

    double end_time = omp_get_wtime();

    double elapsed_time = end_time - start_time;
    int minutes = (int)(elapsed_time / 60);
    int seconds = (int)elapsed_time % 60;
    printf("\nRender completed in %d minutes and %d seconds.\n", minutes, seconds);

    // cleanup
    fclose(img);
    hittable_list_destroy(world);
    bvh_node_free(bvh_world);


    return 0;
}
