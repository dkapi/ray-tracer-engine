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
    camera.aspect_ratio      = 16.0 / 9.0;
    camera.image_width       = 400;  
    camera.samples_per_pixel = 400; 
    camera.max_depth         = 50;
    camera.vfov              = 40;  
    camera.lookfrom          = vec3_create_values(-10.0, 4.0, 4.0); 
    camera.lookat            = vec3_create_values(0.0, 0.0, 0.0); 
    camera.vup               = vec3_create_values(0.0, 1.0, 0.0); 
    camera.defocus_angle     = 0.0; 
    camera.focus_dist        = 1.0;
    camera_initialize(&camera);

    // image output
    FILE *img = fopen("output.ppm", "wb");
    
    // raster
    pixel_t** raster = raster_init(camera.image_width, camera.image_height);

    // world
    hittable_list* world = hittable_list_create();

    // Create Perlin noise texture with a high scale for marbling
    noise_texture_t* marble_texture = create_noise_texture(5.0); // Adjust scale for frequency

    // Add a ground sphere with marble texture
    sphere_t* ground_sphere = sphere_create(&(point3){0, -1000, 0}, 1000, (material_t*)create_lambertian_texture((texture_t*)marble_texture));
    hittable_list_add(world, (hittable*)ground_sphere);

    // Add a smaller sphere with marble texture
    sphere_t* small_sphere = sphere_create(&(point3){0, 2, 0}, 2, (material_t*)create_lambertian_texture((texture_t*)marble_texture));
    hittable_list_add(world, (hittable*)small_sphere);
    hittable_list_add(world, (hittable*)small_sphere);
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
